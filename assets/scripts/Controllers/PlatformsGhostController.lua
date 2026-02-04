-- ========================================
-- NINJA GAIDEN GHOST CONTROLLER
-- Estilo Hollow Knight: Ghost collider + movimiento manual
-- Sin RigidBody, control total desde script
-- ========================================

-- CONFIGURACION DE MOVIMIENTO
local GRAVITY = 35.0
local MAX_FALL_SPEED = 25.0
local FALL_GRAVITY_MULT = 1.5

-- VELOCIDAD INTERNA (no usamos Bullet)
local velocity = { x = 0, y = 0 }

-- TECLAS
local Keys = {
    LEFT = "A",
    RIGHT = "D",
    UP = "W",
    DOWN = "S",
    JUMP = "SPACE",
    ATTACK = "J"
}

-- ESTADOS
local State = {
    IDLE = "idle",
    RUNNING = "running",
    JUMPING = "jumping",
    FALLING = "falling",
    WALL_SLIDING = "wall_sliding"
}

-- VARIABLES DE ESTADO
local currentState = State.IDLE
local facingDirection = 1
local rotationRight = nil
local rotationLeft = nil

local isGrounded = false
local wasGrounded = false
local isOnWall = false
local wallDirection = 0

local coyoteTimeCounter = 0
local jumpBufferCounter = 0

local floor = nil
local rayPositions = {}

-- ========================================
-- INICIALIZACION
-- ========================================

function onStart()
    print("[NinjaGaidenGhost] Controller initialized!")
    print("[NinjaGaidenGhost] Using GHOST collider - manual physics")

    rotationRight = M3:getMatrixRotationForEulerAngles(0, 0, 0)
    rotationLeft = M3:getMatrixRotationForEulerAngles(180, 0, 0)
    this:setRotation(rotationRight)

    floor = Brakeza:getObjectByName("floor")
    if not floor then
        print("[NinjaGaidenGhost] WARNING: No 'floor' object found!")
    end

    -- Resetear velocidad
    velocity.x = 0
    velocity.y = 0

    currentState = State.IDLE
    facingDirection = 1

    print("[NinjaGaidenGhost] Ready!")
end

-- ========================================
-- SISTEMA DE RAYCASTS
-- ========================================

local function updateRayPositions()
    local pos = this:getPosition()
    local halfWidth = colliderRadius
    local halfHeight = colliderHeight * 0.5

    -- Rays de suelo (3 puntos: centro, izquierda, derecha)
    rayPositions.groundCenter = pos - Vertex3D.new(0, halfHeight, 0)
    rayPositions.groundDown = rayPositions.groundCenter - Vertex3D.new(0, groundRayLength, 0)

    rayPositions.groundLeft = pos + Vertex3D.new(halfWidth * 0.8, -halfHeight, 0)
    rayPositions.groundDownLeft = rayPositions.groundLeft - Vertex3D.new(0, groundRayLength, 0)

    rayPositions.groundRight = pos + Vertex3D.new(-halfWidth * 0.8, -halfHeight, 0)
    rayPositions.groundDownRight = rayPositions.groundRight - Vertex3D.new(0, groundRayLength, 0)

    -- Rays de techo (para detectar golpes arriba)
    rayPositions.ceilingCenter = pos + Vertex3D.new(0, halfHeight, 0)
    rayPositions.ceilingUp = rayPositions.ceilingCenter + Vertex3D.new(0, ceilingRayLength, 0)

    -- Rays de pared (a la altura de la cintura)
    rayPositions.wallLeftFrom = pos + Vertex3D.new(0, 0, 0)
    rayPositions.wallLeftTo = rayPositions.wallLeftFrom + Vertex3D.new(halfWidth + wallRayLength, 0, 0)

    rayPositions.wallRightFrom = pos + Vertex3D.new(0, 0, 0)
    rayPositions.wallRightTo = rayPositions.wallRightFrom + Vertex3D.new(-halfWidth - wallRayLength, 0, 0)
end

-- ========================================
-- DETECCION DE COLISIONES
-- ========================================

local function checkGround()
    if not floor then return false end

    local collisions = Components:Collisions()
    local groundCenter = collisions:isRayCollisionWith(rayPositions.groundCenter, rayPositions.groundDown, floor)
    local groundLeft = collisions:isRayCollisionWith(rayPositions.groundLeft, rayPositions.groundDownLeft, floor)
    local groundRight = collisions:isRayCollisionWith(rayPositions.groundRight, rayPositions.groundDownRight, floor)

    return groundCenter or groundLeft or groundRight
end

local function checkCeiling()
    if not floor then return false end

    local collisions = Components:Collisions()
    return collisions:isRayCollisionWith(rayPositions.ceilingCenter, rayPositions.ceilingUp, floor)
end

local function checkWalls()
    if not floor then return false, 0 end

    local collisions = Components:Collisions()
    local wallLeft = collisions:isRayCollisionWith(rayPositions.wallLeftFrom, rayPositions.wallLeftTo, floor)
    local wallRight = collisions:isRayCollisionWith(rayPositions.wallRightFrom, rayPositions.wallRightTo, floor)

    if wallLeft then
        return true, -1
    elseif wallRight then
        return true, 1
    end
    return false, 0
end

local function updateCollisionFlags()
    wasGrounded = isGrounded

    -- Solo considerar en suelo si no estamos subiendo
    local groundHit = checkGround()
    isGrounded = groundHit and velocity.y <= 0

    -- Paredes solo en el aire
    if not isGrounded then
        isOnWall, wallDirection = checkWalls()
    else
        isOnWall = false
        wallDirection = 0
    end
end

-- ========================================
-- MOVIMIENTO HORIZONTAL
-- ========================================

local function handleHorizontalMovement(input, dt)
    local moveInput = 0

    if input:isCharPressed(Keys.LEFT) then
        moveInput = -1
    elseif input:isCharPressed(Keys.RIGHT) then
        moveInput = 1
    end

    -- Actualizar direccion visual
    if moveInput ~= 0 then
        facingDirection = moveInput
        this:setRotation(facingDirection > 0 and rotationRight or rotationLeft)
    end

    -- Calcular velocidad objetivo
    local targetVelX = moveInput * moveSpeed

    -- Aceleracion/desaceleracion suave
    local accel = isGrounded and groundAcceleration or airAcceleration

    if moveInput ~= 0 then
        -- Acelerando hacia direccion de input
        if velocity.x * moveInput < targetVelX * moveInput then
            velocity.x = velocity.x + moveInput * accel * dt
            if velocity.x * moveInput > targetVelX * moveInput then
                velocity.x = targetVelX
            end
        end
    else
        -- Frenando (friccion)
        local friction = isGrounded and groundFriction or 0.98
        velocity.x = velocity.x * friction
        if math.abs(velocity.x) < 0.1 then
            velocity.x = 0
        end
    end
end

-- ========================================
-- SISTEMA DE SALTO
-- ========================================

local function handleJump(input, dt)
    -- Coyote time
    if isGrounded then
        coyoteTimeCounter = coyoteTimeFrames
    elseif coyoteTimeCounter > 0 then
        coyoteTimeCounter = coyoteTimeCounter - 1
    end

    -- Jump buffer
    local jumpPressed = input:isCharFirstEventDown(Keys.JUMP)
    local jumpHeld = input:isCharPressed(Keys.JUMP)

    if jumpPressed then
        jumpBufferCounter = jumpBufferFrames
    elseif jumpBufferCounter > 0 then
        jumpBufferCounter = jumpBufferCounter - 1
    end

    -- Ejecutar salto
    local canJump = coyoteTimeCounter > 0 or isGrounded

    if jumpBufferCounter > 0 and canJump then
        velocity.y = jumpForce
        coyoteTimeCounter = 0
        jumpBufferCounter = 0
        isGrounded = false
        print("[NinjaGaidenGhost] JUMP!")
    end

    -- Wall jump
    if jumpPressed and isOnWall and not isGrounded then
        velocity.x = -wallDirection * wallJumpForceX
        velocity.y = wallJumpForceY
        facingDirection = -wallDirection
        this:setRotation(facingDirection > 0 and rotationRight or rotationLeft)
        print("[NinjaGaidenGhost] WALL JUMP!")
    end

    -- Variable jump height (soltar = cortar salto)
    if not jumpHeld and velocity.y > 0 then
        velocity.y = velocity.y * jumpCutMultiplier
    end
end

-- ========================================
-- SISTEMA DE GRAVEDAD
-- ========================================

local function handleGravity(input, dt)
    if isGrounded then
        -- En suelo: sin gravedad, velocidad Y = 0
        if velocity.y < 0 then
            velocity.y = 0
        end
        return
    end

    -- Wall slide
    if isOnWall and velocity.y < 0 then
        velocity.y = math.max(velocity.y, -wallSlideSpeed)
        return
    end

    -- Gravedad normal
    local grav = GRAVITY

    -- Gravedad aumentada en caida
    if velocity.y < 0 then
        grav = grav * FALL_GRAVITY_MULT
    end

    -- Fast fall
    if input:isCharPressed(Keys.DOWN) and velocity.y < 0 then
        grav = grav * 1.5
    end

    velocity.y = velocity.y - grav * dt

    -- Limitar velocidad de caida
    if velocity.y < -MAX_FALL_SPEED then
        velocity.y = -MAX_FALL_SPEED
    end
end

-- ========================================
-- MOVIMIENTO Y RESOLUCION DE COLISIONES
-- ========================================

local function findGround(x, fromY, toY, z, collisions, halfHeight)
    -- Busca la Y válida más alta donde el personaje puede estar
    local rayFrom = Vertex3D.new(x, fromY, z)
    local rayTo = Vertex3D.new(x, toY - halfHeight, z)

    if not collisions:isRayCollisionWith(rayFrom, rayTo, floor) then
        return nil
    end

    -- Búsqueda binaria: encontrar Y donde los pies tocan el suelo
    local top = fromY
    local bottom = toY
    for i = 1, 14 do
        local mid = (top + bottom) * 0.5
        local testFrom = Vertex3D.new(x, mid, z)
        local testTo = Vertex3D.new(x, mid - halfHeight - 0.02, z)
        if collisions:isRayCollisionWith(testFrom, testTo, floor) then
            bottom = mid
        else
            top = mid
        end
    end
    return top
end

local function isWall(fromX, toX, y, z, halfWidth, halfHeight, collisions)
    -- Detectar pared: raycast horizontal a múltiples alturas
    -- Cabeza (90%)
    local rayFrom1 = Vertex3D.new(fromX, y + halfHeight * 0.9, z)
    local rayTo1 = Vertex3D.new(toX, y + halfHeight * 0.9, z)
    if collisions:isRayCollisionWith(rayFrom1, rayTo1, floor) then
        return true
    end
    -- Cintura (50%)
    local rayFrom2 = Vertex3D.new(fromX, y + halfHeight * 0.5, z)
    local rayTo2 = Vertex3D.new(toX, y + halfHeight * 0.5, z)
    if collisions:isRayCollisionWith(rayFrom2, rayTo2, floor) then
        return true
    end
    return false
end

local function moveAndCollide(dt)
    local pos = this:getPosition()
    local collisions = Components:Collisions()

    local newX = pos.x
    local newY = pos.y
    local newZ = pos.z  -- Preservar Z original

    local halfWidth = colliderRadius
    local halfHeight = colliderHeight * 0.5

    -- ===== 1. MOVIMIENTO HORIZONTAL + RAMPAS =====
    local moveX = velocity.x * dt
    if moveX ~= 0 and floor then
        local targetX = pos.x + moveX
        local dir = moveX > 0 and 1 or -1

        if isGrounded then
            -- EN SUELO: buscar si hay suelo válido en destino
            local slopeRange = math.abs(moveX) * 2.0 + 0.5  -- Permite rampas hasta ~63°
            local groundY = findGround(targetX, pos.y + slopeRange, pos.y - slopeRange, newZ, collisions, halfHeight)

            if groundY then
                -- Hay suelo: verificar que no sea una pared (cabeza libre)
                if not isWall(pos.x, targetX + dir * halfWidth, groundY, newZ, halfWidth, halfHeight, collisions) then
                    newX = targetX
                    newY = groundY
                end
                -- Si es pared, no movemos (newX = pos.x)
            else
                -- No hay suelo: caminar hasta el borde y caer
                newX = targetX
                isGrounded = false
            end
        else
            -- EN AIRE: solo detectar paredes
            if not isWall(pos.x, targetX + dir * halfWidth, pos.y, newZ, halfWidth, halfHeight, collisions) then
                newX = targetX
            end
        end
    end

    -- ===== 2. SNAP AL SUELO (sin movimiento horizontal) =====
    if isGrounded and velocity.y <= 0 and moveX == 0 and floor then
        local groundY = findGround(newX, pos.y + 0.3, pos.y - 0.3, newZ, collisions, halfHeight)
        if groundY then
            newY = groundY
        end
    end

    -- ===== 3. MOVIMIENTO VERTICAL (aire) =====
    if not isGrounded or velocity.y > 0 then
        local moveY = velocity.y * dt

        if velocity.y > 0 then
            -- SUBIENDO
            local targetY = newY + moveY
            local rayFrom = Vertex3D.new(newX, newY + halfHeight, newZ)
            local rayTo = Vertex3D.new(newX, targetY + halfHeight, newZ)

            if collisions:isRayCollisionWith(rayFrom, rayTo, floor) then
                velocity.y = 0
            else
                newY = targetY
            end
        else
            -- CAYENDO
            local targetY = newY + moveY
            local groundY = findGround(newX, newY, targetY, newZ, collisions, halfHeight)

            if groundY and groundY >= targetY then
                newY = groundY
                velocity.y = 0
                isGrounded = true
            else
                newY = targetY
            end
        end
    end

    -- ===== 4. VERIFICACIÓN ANTI-CLIP Y DESATASCO =====
    if floor then
        -- Verificar si hay colisión a los lados (atascado en geometría)
        local leftCheck = Vertex3D.new(newX - halfWidth, newY, newZ)
        local rightCheck = Vertex3D.new(newX + halfWidth, newY, newZ)
        local centerPoint = Vertex3D.new(newX, newY, newZ)

        local blockedLeft = collisions:isRayCollisionWith(centerPoint, leftCheck, floor)
        local blockedRight = collisions:isRayCollisionWith(centerPoint, rightCheck, floor)

        -- Si está bloqueado en un lado, empujar hacia el otro
        if blockedLeft and not blockedRight then
            -- Empujar hacia la derecha
            for i = 1, 10 do
                local testX = newX + i * 0.05
                local testFrom = Vertex3D.new(testX, newY, newZ)
                local testLeft = Vertex3D.new(testX - halfWidth, newY, newZ)
                if not collisions:isRayCollisionWith(testFrom, testLeft, floor) then
                    newX = testX
                    break
                end
            end
        elseif blockedRight and not blockedLeft then
            -- Empujar hacia la izquierda
            for i = 1, 10 do
                local testX = newX - i * 0.05
                local testFrom = Vertex3D.new(testX, newY, newZ)
                local testRight = Vertex3D.new(testX + halfWidth, newY, newZ)
                if not collisions:isRayCollisionWith(testFrom, testRight, floor) then
                    newX = testX
                    break
                end
            end
        elseif blockedLeft and blockedRight then
            -- Bloqueado en ambos lados: volver a posición anterior
            newX = pos.x
            newY = pos.y
        end

        -- Después del desatasco, re-buscar suelo
        if isGrounded then
            local groundY = findGround(newX, newY + 0.5, newY - 0.5, newZ, collisions, halfHeight)
            if groundY then
                newY = groundY
            end
        end
    end

    this:setPosition(Vertex3D.new(newX, newY, newZ))
end

-- ========================================
-- ACTUALIZACION DE ESTADO
-- ========================================

local function updateState()
    if isOnWall and not isGrounded and velocity.y < 0 then
        currentState = State.WALL_SLIDING
    elseif isGrounded then
        if math.abs(velocity.x) > 0.5 then
            currentState = State.RUNNING
        else
            currentState = State.IDLE
        end
    else
        if velocity.y > 0 then
            currentState = State.JUMPING
        else
            currentState = State.FALLING
        end
    end
end

-- ========================================
-- DEBUG VISUAL
-- ========================================

local debugTextWriter = nil

local function debugDraw()
    local render = Components:Render()
    local green = Color.new(0, 1, 0, 1)
    local red = Color.new(1, 0, 0, 1)
    local yellow = Color.new(1, 1, 0, 1)
    local cyan = Color.new(0, 1, 1, 1)
    local magenta = Color.new(1, 0, 1, 1)
    local orange = Color.new(1, 0.5, 0, 1)

    local pos = this:getPosition()

    -- Flecha de velocidad
    local velScale = 0.1
    local velEnd = pos + Vertex3D.new(velocity.x * velScale, velocity.y * velScale, 0)
    render:DrawLine(pos, velEnd, cyan)

    -- Rays de suelo
    local groundColor = isGrounded and green or red
    render:DrawLine(rayPositions.groundCenter, rayPositions.groundDown, groundColor)
    render:DrawLine(rayPositions.groundLeft, rayPositions.groundDownLeft, groundColor)
    render:DrawLine(rayPositions.groundRight, rayPositions.groundDownRight, groundColor)

    -- Rays de techo
    render:DrawLine(rayPositions.ceilingCenter, rayPositions.ceilingUp, yellow)

    -- Rays de pared
    local wallLeftColor = (isOnWall and wallDirection == -1) and green or red
    local wallRightColor = (isOnWall and wallDirection == 1) and green or red
    render:DrawLine(rayPositions.wallLeftFrom, rayPositions.wallLeftTo, wallLeftColor)
    render:DrawLine(rayPositions.wallRightFrom, rayPositions.wallRightTo, wallRightColor)

    -- Texto debug
    if not debugTextWriter then
        debugTextWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    end

    local y = 20
    local white = Color.new(1, 1, 1, 1)

    debugTextWriter:writeTextTTFAutoSize(20, y, "[GHOST MODE]", cyan, 0.9)
    y = y + 25

    debugTextWriter:writeTextTTFAutoSize(20, y, "STATE: " .. currentState, white, 0.9)
    y = y + 25

    debugTextWriter:writeTextTTFAutoSize(20, y, "VEL X: " .. string.format("%.2f", velocity.x), magenta, 0.9)
    y = y + 20
    debugTextWriter:writeTextTTFAutoSize(20, y, "VEL Y: " .. string.format("%.2f", velocity.y), orange, 0.9)
    y = y + 25

    debugTextWriter:writeTextTTFAutoSize(20, y, "GROUNDED: " .. (isGrounded and "YES" or "NO"), isGrounded and green or red, 0.9)
    y = y + 25

    debugTextWriter:writeTextTTFAutoSize(20, y, "ON WALL: " .. (isOnWall and "YES" or "NO"), isOnWall and green or red, 0.9)
    y = y + 25

    debugTextWriter:writeTextTTFAutoSize(20, y, "COYOTE: " .. coyoteTimeCounter, yellow, 0.9)
end

-- ========================================
-- LOOP PRINCIPAL
-- ========================================

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()

    -- 1. Actualizar posiciones de raycast
    updateRayPositions()

    -- 2. Detectar colisiones
    updateCollisionFlags()

    -- 3. Procesar input
    handleHorizontalMovement(input, dt)
    handleJump(input, dt)
    handleGravity(input, dt)

    -- 4. Mover y resolver colisiones
    moveAndCollide(dt)

    -- 5. Actualizar estado
    updateState()

    -- 6. Debug
    debugDraw()
end

-- ========================================
-- EVENTOS
-- ========================================

function onCollision(with)
    -- No usamos colisiones de Bullet, solo raycasts
end

function onEnd()
    print("[NinjaGaidenGhost] Controller ended!")
end
