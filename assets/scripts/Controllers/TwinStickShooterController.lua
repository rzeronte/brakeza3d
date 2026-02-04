-- ============================================================================
-- TWIN-STICK SHOOTER CONTROLLER
-- Controlador para juegos de disparos top-down/isometrico
-- Movimiento: WASD | Apuntar: Mouse | Disparar: Click Izq | Dash: Click Der
-- ============================================================================

-- CONFIGURACION DE TECLAS
local Keys = {
    FORWARD = "W",
    BACKWARD = "S",
    LEFT = "A",
    RIGHT = "D",
    RELOAD = "R",
    INTERACT = "E"
}

-- ESTADOS DEL PERSONAJE
local State = {
    IDLE = "idle",
    MOVING = "moving",
    DASHING = "dashing",
    SHOOTING = "shooting",
    RELOADING = "reloading"
}

-- VARIABLES DE ESTADO
local currentState = State.IDLE
local facingDirection = Vertex3D.new(1, 0, 0)
local targetRotation = nil

-- TIMERS
local fireTimer = 0
local dashTimer = 0
local dashCooldownTimer = 0
local reloadTimer = 0

-- ESTADO DE ARMA
local currentAmmo = 30
local isReloading = false

-- DASH
local isDashing = false
local dashDirection = Vertex3D.new(0, 0, 0)

-- PROYECTILES ACTIVOS (pool simple)
local bullets = {}
local maxBullets = 50

-- CACHE DE OBJETOS
local camera = nil
local floor = nil

-- SISTEMA DE TEXTO
local textWriter = nil

-- ============================================================================
-- INICIALIZACION
-- ============================================================================

function onStart()
    print("[TwinStick] Controller initialized!")

    -- Obtener referencias
    camera = Components:Camera():getCamera()
    floor = Brakeza:getObjectByName("floor")

    -- Crear TextWriter para UI
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)

    -- Configurar fisicas
    this:setAngularFactor(Vertex3D.new(0, 0, 0))  -- No rotar por fisicas
    this:setFriction(0.9)
    this:setRestitution(0)
    this:setLinearDamping(0.5)  -- Desaceleracion natural

    -- Inicializar municion
    currentAmmo = maxAmmo

    -- Estado inicial
    currentState = State.IDLE
    targetRotation = M3:getMatrixRotationForEulerAngles(0, 0, 0)

    print("[TwinStick] Ready! Ammo: " .. currentAmmo .. "/" .. maxAmmo)
end

-- ============================================================================
-- SISTEMA DE APUNTADO (MOUSE TO WORLD)
-- ============================================================================

local function getMouseWorldPosition()
    local input = Components:Input()
    local window = Components:Window()

    -- Obtener posicion del mouse en pantalla (0-1 normalizado)
    local mouseX = input:getRelativeRendererMouseX()
    local mouseY = input:getRelativeRendererMouseY()

    -- Obtener dimensiones de la ventana de render
    local screenW = window:getWidthRender()
    local screenH = window:getHeightRender()

    -- Convertir a coordenadas de pantalla
    local screenPosX = mouseX * screenW
    local screenPosY = mouseY * screenH

    -- Proyectar rayo desde la camara
    -- Usamos una aproximacion basada en la posicion del jugador y la camara
    local playerPos = this:getPosition()
    local camPos = camera:getPosition()

    -- Calcular direccion del rayo (simplificado para vista top-down)
    -- En una vista top-down, el mouse X/Y se mapea a mundo X/Z
    local worldX = playerPos.x + (mouseX - 0.5) * cameraHeight * 2
    local worldZ = playerPos.z + (mouseY - 0.5) * cameraHeight * 2

    return Vertex3D.new(worldX, movementPlaneY, worldZ)
end

local function getAimDirection()
    local playerPos = this:getPosition()
    local mouseWorld = getMouseWorldPosition()

    -- Direccion desde el jugador hacia el mouse
    local dir = mouseWorld - playerPos
    dir.y = 0  -- Mantener en plano horizontal

    local length = dir:getModule()
    if length > 0.001 then
        return dir:getScaled(1.0 / length)  -- Normalizar
    end

    return facingDirection
end

-- ============================================================================
-- SISTEMA DE MOVIMIENTO
-- ============================================================================

local function handleMovement(input, dt)
    -- No mover durante dash
    if isDashing then return end

    local moveDir = Vertex3D.new(0, 0, 0)

    -- Leer input WASD
    if input:isCharPressed(Keys.FORWARD) then
        moveDir.z = moveDir.z + 1
    end
    if input:isCharPressed(Keys.BACKWARD) then
        moveDir.z = moveDir.z - 1
    end
    if input:isCharPressed(Keys.LEFT) then
        moveDir.x = moveDir.x + 1
    end
    if input:isCharPressed(Keys.RIGHT) then
        moveDir.x = moveDir.x - 1
    end

    -- Normalizar diagonal
    local length = moveDir:getModule()
    if length > 0.001 then
        moveDir = moveDir:getScaled(1.0 / length)
        currentState = State.MOVING
    else
        currentState = State.IDLE
    end

    -- Aplicar velocidad
    local vel = this:getLinearVelocity()
    vel.x = moveDir.x * moveSpeed
    vel.z = moveDir.z * moveSpeed
    this:setLinearVelocity(vel)
end

-- ============================================================================
-- SISTEMA DE ROTACION (MIRAR AL MOUSE)
-- ============================================================================

local function handleRotation(input, dt)
    -- Obtener direccion de apuntado
    local aimDir = getAimDirection()
    facingDirection = aimDir

    -- Calcular angulo de rotacion (en Y)
    local angle = math.atan2(aimDir.x, aimDir.z)
    local angleDegrees = math.deg(angle)

    -- Crear rotacion suave
    local targetRot = M3:getMatrixRotationForEulerAngles(0, angleDegrees, 0)

    -- Aplicar rotacion (con suavizado)
    -- Por simplicidad, aplicamos directamente
    this:setRotation(targetRot)
end

-- ============================================================================
-- SISTEMA DE DASH
-- ============================================================================

local function handleDash(input, dt)
    -- Actualizar cooldown
    if dashCooldownTimer > 0 then
        dashCooldownTimer = dashCooldownTimer - dt
    end

    -- Ejecutar dash activo
    if isDashing then
        dashTimer = dashTimer - dt

        -- Aplicar velocidad de dash
        local vel = dashDirection:getScaled(dashSpeed)
        vel.y = this:getLinearVelocity().y  -- Mantener gravedad
        this:setLinearVelocity(vel)

        if dashTimer <= 0 then
            isDashing = false
            currentState = State.IDLE
            print("[TwinStick] Dash ended!")
        end
        return
    end

    -- Iniciar dash con click derecho
    if input:isClickRight() and dashCooldownTimer <= 0 then
        -- Direccion del dash = direccion de movimiento actual o facing
        local vel = this:getLinearVelocity()
        local moveDir = Vertex3D.new(vel.x, 0, vel.z)
        local length = moveDir:getModule()

        if length > 0.5 then
            dashDirection = moveDir:getScaled(1.0 / length)
        else
            dashDirection = facingDirection
        end

        isDashing = true
        dashTimer = dashDuration
        dashCooldownTimer = dashCooldown
        currentState = State.DASHING

        print("[TwinStick] Dash!")
    end
end

-- ============================================================================
-- SISTEMA DE DISPARO
-- ============================================================================

local function spawnBullet()
    local pos = this:getPosition() + facingDirection:getScaled(1.0)
    pos.y = pos.y + 0.5  -- Elevar un poco

    -- Crear proyectil como Mesh3D simple
    local bullet = ObjectFactory.Mesh3D()
    bullet:setName("bullet")
    bullet:AssimpLoadGeometryFromFile("../assets/models/basic/icosphere.fbx")
    bullet:setPosition(pos)
    bullet:setScale(0.15)
    bullet:setBelongToScene(false)
    bullet:setEnableLights(false)

    -- Configurar como rigidbody
    bullet:SetupRigidBodyCollider(0.1, SIMPLE_SHAPE)
    bullet:setGravityCollider(Vertex3D.new(0, 0, 0))  -- Sin gravedad
    bullet:setCollisionsEnabled(true)

    -- Aplicar velocidad
    local vel = facingDirection:getScaled(bulletSpeed)
    bullet:setLinearVelocity(vel)

    -- Agregar al mundo
    Brakeza:AddObject3D(bullet)

    -- Guardar en pool con timestamp
    table.insert(bullets, {
        object = bullet,
        lifetime = bulletLifetime
    })

    -- Limitar cantidad de balas
    if #bullets > maxBullets then
        local old = table.remove(bullets, 1)
        old.object:setRemoved(true)
    end
end

local function handleShooting(input, dt)
    -- No disparar durante recarga
    if isReloading then return end

    -- Actualizar fire timer
    if fireTimer > 0 then
        fireTimer = fireTimer - dt
    end

    -- Disparar con click izquierdo
    if input:isLeftMouseButtonPressed() and fireTimer <= 0 and currentAmmo > 0 then
        --spawnBullet()
        currentAmmo = currentAmmo - 1
        fireTimer = fireRate
        currentState = State.SHOOTING

        -- Sonido de disparo (si existe)
        -- Components:Sound():PlaySound("shoot", 0, 0)
    end

    -- Auto-reload cuando se acaba la municion
    if currentAmmo <= 0 and not isReloading then
        isReloading = true
        reloadTimer = reloadTime
        currentState = State.RELOADING
        print("[TwinStick] Reloading...")
    end
end

local function handleReload(input, dt)
    -- Recarga manual con R
    if input:isCharFirstEventDown(Keys.RELOAD) and not isReloading and currentAmmo < maxAmmo then
        isReloading = true
        reloadTimer = reloadTime
        currentState = State.RELOADING
        print("[TwinStick] Reloading...")
    end

    -- Procesar recarga activa
    if isReloading then
        reloadTimer = reloadTimer - dt

        if reloadTimer <= 0 then
            currentAmmo = maxAmmo
            isReloading = false
            currentState = State.IDLE
            print("[TwinStick] Reload complete! Ammo: " .. currentAmmo)
        end
    end
end

-- ============================================================================
-- ACTUALIZACION DE PROYECTILES
-- ============================================================================

local function updateBullets(dt)
    local toRemove = {}

    for i, bullet in ipairs(bullets) do
        bullet.lifetime = bullet.lifetime - dt

        if bullet.lifetime <= 0 or not bullet.object:isEnabled() then
            table.insert(toRemove, i)
            bullet.object:setRemoved(true)
        end
    end

    -- Eliminar balas expiradas (en orden inverso)
    for i = #toRemove, 1, -1 do
        table.remove(bullets, toRemove[i])
    end
end

-- ============================================================================
-- INTERFAZ DE USUARIO
-- ============================================================================

local function drawUI()
    local window = Components:Window()
    local screenW = window:getWidth()
    local screenH = window:getHeight()

    -- Municion
    local ammoColor = Color.new(0, 1, 0, 1)
    if currentAmmo <= 5 then
        ammoColor = Color.new(1, 0.3, 0, 1)  -- Naranja bajo
    end
    if currentAmmo == 0 then
        ammoColor = Color.new(1, 0, 0, 1)  -- Rojo vacio
    end

    local ammoText = "AMMO: " .. currentAmmo .. "/" .. maxAmmo
    if isReloading then
        local progress = math.floor((1 - reloadTimer / reloadTime) * 100)
        ammoText = "RELOADING... " .. progress .. "%"
        ammoColor = Color.new(1, 1, 0, 1)
    end

    textWriter:writeTextTTFAutoSize(20, screenH - 50, ammoText, ammoColor, 1.2)

    -- Dash cooldown
    local dashReady = dashCooldownTimer <= 0
    local dashColor = dashReady and Color.new(0, 1, 1, 1) or Color.new(0.5, 0.5, 0.5, 1)
    --local dashText = dashReady and "DASH: READY" or "DASH: " .. string.format("%.1f", dashCooldownTimer)
    --textWriter:writeTextTTFAutoSize(20, screenH - 80, dashText, dashColor, 1.0)

    -- Estado actual
    --local stateText = "STATE: " .. string.upper(currentState)
    --textWriter:writeTextTTFAutoSize(20, screenH - 110, stateText, Color.new(0.7, 0.7, 0.7, 1), 0.8)

    -- Controles
    textWriter:writeTextTTFAutoSize(screenW - 250, 20, "WASD = Move", Color.new(0.5, 0.5, 0.5, 1), 0.8)
    textWriter:writeTextTTFAutoSize(screenW - 250, 45, "Mouse = Aim", Color.new(0.5, 0.5, 0.5, 1), 0.8)
    textWriter:writeTextTTFAutoSize(screenW - 250, 70, "LMB = Shoot", Color.new(0.5, 0.5, 0.5, 1), 0.8)
    textWriter:writeTextTTFAutoSize(screenW - 250, 95, "RMB = Dash", Color.new(0.5, 0.5, 0.5, 1), 0.8)
    textWriter:writeTextTTFAutoSize(screenW - 250, 120, "R = Reload", Color.new(0.5, 0.5, 0.5, 1), 0.8)
end

-- ============================================================================
-- DEBUG VISUAL
-- ============================================================================

local function debugDraw()
    local render = Components:Render()
    local pos = this:getPosition()
    pos.y = pos.y + 0.5

    -- Linea de apuntado
    local aimEnd = pos + facingDirection:getScaled(aimLineLength)
    local aimColor = Color.new(1, 0, 0, 0.8)
    if isReloading then
        aimColor = Color.new(1, 1, 0, 0.5)
    end
    render:DrawLine(pos, aimEnd, aimColor)

    -- Indicador de dash disponible
    if dashCooldownTimer <= 0 then
        local dashIndicator = pos + Vertex3D.new(0, 1.5, 0)
        render:DrawLine(pos + Vertex3D.new(0, 1.3, 0), dashIndicator, Color.new(0, 1, 1, 1))
    end
end

-- ============================================================================
-- LOOP PRINCIPAL
-- ============================================================================

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local input = Components:Input()

    -- Sistemas principales
    handleMovement(input, dt)
    handleRotation(input, dt)
    handleDash(input, dt)
    handleShooting(input, dt)
    handleReload(input, dt)

    -- Actualizar proyectiles
    updateBullets(dt)

    -- Interfaz y debug
    drawUI()
    debugDraw()
end

-- ============================================================================
-- EVENTOS
-- ============================================================================

function onCollision(with)
    -- Aqui se puede manejar dano recibido, recoger items, etc.
end

function onEnd()
    -- Limpiar proyectiles
    for _, bullet in ipairs(bullets) do
        bullet.object:setRemoved(true)
    end
    bullets = {}

    print("[TwinStick] Controller ended!")
end
