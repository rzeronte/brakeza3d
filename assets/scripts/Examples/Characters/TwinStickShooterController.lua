-- ============================================================================
-- TWIN-STICK SHOOTER CONTROLLER
-- Type: Object Script (attach to the player: a rigid body with its angular factor locked)
-- ============================================================================
-- Top-down shooter with TWIN-STICK controls: one stick moves, the other one aims.
--
--   GAMEPAD                                   KEYBOARD + MOUSE
--   left stick     move                       W A S D   move
--   right stick    aim                        mouse     aim
--   RT or RB       shoot (hold)               left button   shoot (hold)
--   LT or A        dash                       right button  dash
--   X              reload                     R             reload
--
-- Both schemes work at the same time; the last one you touch decides what the HUD shows.
--
-- Directions are always in SCREEN space ("up" on the stick = up on the screen) and converted to the world with
-- the camera, so it works for any fixed top-down or tilted camera: the script projects the player and two points
-- one unit away to the screen and solves for the world direction (see screenToWorldDir).
--
-- Bullets are plain meshes moved by the script (no physics) and checked against the targets "Dummy_1..N" by distance.
-- The camera follows the player from above (cameraHeight, cameraDistance).
-- ============================================================================

local function num(v, default)
    v = tonumber(v)
    if v == nil then return default end
    return v
end

local function clamp(v, lo, hi)
    return math.max(lo, math.min(hi, v))
end

-- ── estado ──────────────────────────────────────────────────────────────────
local input, camera, window, textWriter
local startPosition = nil
local facing = Vertex3D.new(0, 0, -1)      -- hacia dónde apunta (mundo, plano XZ)
local lastMouseX, lastMouseY = -1, -1
local mouseAimActive = false               -- true si el último que apuntó fue el ratón
local usingPad = false                     -- true si el último que se tocó fue el mando

local fireTimer, dashTimer, dashCooldownTimer, reloadTimer = 0, 0, 0, 0
local isDashing, isReloading = false, false
local dashDirection = Vertex3D.new(0, 0, 0)
local currentAmmo = 30

local bullets = {}                         -- { obj, dir, life }
local bulletCounter = 0
local muzzle = nil                         -- cubo pequeño que marca hacia dónde mira el jugador

local dummies = {}                         -- { obj, hp, hitFlash, respawn }
local shots, kills = 0, 0

-- ── proyección pantalla <-> mundo ───────────────────────────────────────────
-- Devuelve la base que forman dos vectores unidad del mundo (X y Z) vistos en pantalla.
local function screenBasis(w, h)
    local cam = Components:Camera()
    local p = this:getPosition()
    local x0, y0 = cam:worldToScreenXYZ(p.x, p.y, p.z, w, h)
    local x1, y1 = cam:worldToScreenXYZ(p.x + 1, p.y, p.z, w, h)
    local x2, y2 = cam:worldToScreenXYZ(p.x, p.y, p.z + 1, w, h)
    local ax, ay, bx, by = x1 - x0, y1 - y0, x2 - x0, y2 - y0
    local det = ax * by - ay * bx
    if math.abs(det) < 1e-6 then return nil end
    return { x0 = x0, y0 = y0, ax = ax, ay = ay, bx = bx, by = by, det = det }
end

-- Un vector en pantalla (mx a la derecha, my hacia abajo) -> dirección en el mundo (plano XZ, normalizada)
local function screenToWorldDir(basis, mx, my)
    if basis == nil then return nil end
    local a = (mx * basis.by - my * basis.bx) / basis.det
    local b = (basis.ax * my - basis.ay * mx) / basis.det
    local len = math.sqrt(a * a + b * b)
    if len < 1e-6 then return nil end
    return Vertex3D.new(a / len, 0, b / len)
end

-- ── entrada: mando y teclado juntos ─────────────────────────────────────────
local function stick(x, y)
    local dz = num(stickDeadzone, 0.25)
    local len = math.sqrt(x * x + y * y)
    if len < dz then return 0, 0 end
    local k = (len - dz) / (1 - dz) / len        -- reescalado: sin salto al salir de la zona muerta
    return x * k, y * k
end

-- Movimiento deseado en pantalla: (sx, sy), longitud 0..1
local function readMoveScreen()
    local sx, sy = 0, 0
    if input:isGameControllerAvailable() then
        sx, sy = stick(input:getControllerAxisLeftX(), input:getControllerAxisLeftY())
        if sx ~= 0 or sy ~= 0 then usingPad = true end
    end
    local kx, ky = 0, 0
    if input:isCharPressed("D") then kx = kx + 1 end
    if input:isCharPressed("A") then kx = kx - 1 end
    if input:isCharPressed("S") then ky = ky + 1 end        -- hacia abajo en pantalla
    if input:isCharPressed("W") then ky = ky - 1 end
    if kx ~= 0 or ky ~= 0 then
        usingPad = false
        local l = math.sqrt(kx * kx + ky * ky)               -- diagonales sin ir más rápido
        return kx / l, ky / l
    end
    return sx, sy
end

local function padAxisAim()
    if not input:isGameControllerAvailable() then return nil end
    local rx, ry = stick(input:getControllerAxisRightX(), input:getControllerAxisRightY())
    if rx == 0 and ry == 0 then return nil end
    return rx, ry
end

local function wantsShoot()
    if input:isLeftMouseButtonPressed() then usingPad = false; return true end
    if input:isGameControllerAvailable() then
        if input:getControllerAxisTriggerRight() > 0.3 or input:getControllerShoulderRight() then
            usingPad = true
            return true
        end
    end
    return false
end

local function wantsDash()
    if input:isClickRight() then usingPad = false; return true end
    if input:isGameControllerAvailable() then
        if input:getControllerAxisTriggerLeft() > 0.3 or input:getControllerButtonA() then
            usingPad = true
            return true
        end
    end
    return false
end

local function wantsReload()
    if input:isCharFirstEventDown("R") then return true end
    return input:isGameControllerAvailable() and input:getControllerButtonX()
end

-- ── apuntado ────────────────────────────────────────────────────────────────
local function updateAim(basis, w, h)
    -- stick derecho: manda mientras se mueve
    local rx, ry = padAxisAim()
    if rx ~= nil then
        local d = screenToWorldDir(basis, rx, ry)
        if d ~= nil then facing = d end
        mouseAimActive = false
        usingPad = true
        return
    end

    -- ratón: manda mientras se mueve (o si lo último que se usó fue el ratón)
    local mx, my = input:getRawMouseX(), input:getRawMouseY()
    if mx ~= lastMouseX or my ~= lastMouseY then
        lastMouseX, lastMouseY = mx, my
        mouseAimActive = true
        usingPad = false
    end
    if mouseAimActive and basis ~= nil then
        local d = screenToWorldDir(basis, mx - basis.x0, my - basis.y0)
        if d ~= nil then facing = d end
    end
end

-- ── movimiento y dash ───────────────────────────────────────────────────────
local function handleMovement(basis)
    if isDashing then return end
    local sx, sy = readMoveScreen()
    local dir = screenToWorldDir(basis, sx, sy)
    local strength = math.min(1, math.sqrt(sx * sx + sy * sy))       -- el stick analógico da velocidades intermedias
    local speed = num(moveSpeed, 8.0) * strength
    local vel = this:getLinearVelocity()
    if dir ~= nil and strength > 0 then
        this:setLinearVelocity(Vertex3D.new(dir.x * speed, vel.y, dir.z * speed))
    else
        this:setLinearVelocity(Vertex3D.new(0, vel.y, 0))
    end
end

local function handleDash(basis, dt)
    if dashCooldownTimer > 0 then dashCooldownTimer = dashCooldownTimer - dt end

    if isDashing then
        dashTimer = dashTimer - dt
        local vel = this:getLinearVelocity()
        local v = dashDirection:getScaled(num(dashSpeed, 25.0))
        this:setLinearVelocity(Vertex3D.new(v.x, vel.y, v.z))
        if dashTimer <= 0 then isDashing = false end
        return
    end

    if wantsDash() and dashCooldownTimer <= 0 then
        -- el dash va hacia donde te mueves; parado, hacia donde apuntas
        local sx, sy = readMoveScreen()
        local d = screenToWorldDir(basis, sx, sy)
        dashDirection = d or facing
        isDashing = true
        dashTimer = num(dashDuration, 0.15)
        dashCooldownTimer = num(dashCooldown, 0.8)
    end
end

-- ── disparo ─────────────────────────────────────────────────────────────────
local function spawnBullet()
    local p = this:getPosition()
    local pos = Vertex3D.new(p.x + facing.x * 1.0, p.y + 0.3, p.z + facing.z * 1.0)
    bulletCounter = bulletCounter + 1

    local bullet = ObjectFactory.Mesh3D("../assets/models/basic/icosphere.fbx", pos)
    bullet:setName("tw_bullet_" .. bulletCounter)
    bullet:setScale(0.18)
    bullet:setSelectable(false)

    bullets[#bullets + 1] = { obj = bullet, dir = Vertex3D.new(facing.x, 0, facing.z), life = num(bulletLifetime, 1.5) }
    -- tope: las más antiguas desaparecen primero
    while #bullets > 60 do
        local old = table.remove(bullets, 1)
        pcall(function() old.obj:setRemoved(true) end)
    end
end

local function startReload()
    if isReloading or currentAmmo >= num(maxAmmo, 30) then return end
    isReloading = true
    reloadTimer = num(reloadTime, 1.5)
end

local function handleShooting(dt)
    if fireTimer > 0 then fireTimer = fireTimer - dt end
    if isReloading then return end

    if wantsShoot() and fireTimer <= 0 and currentAmmo > 0 then
        spawnBullet()
        currentAmmo = currentAmmo - 1
        shots = shots + 1
        fireTimer = num(fireRate, 0.12)
    end

    if currentAmmo <= 0 then startReload() end
end

local function handleReload(dt)
    if wantsReload() then startReload() end
    if isReloading then
        reloadTimer = reloadTimer - dt
        if reloadTimer <= 0 then
            currentAmmo = num(maxAmmo, 30)
            isReloading = false
        end
    end
end

-- ── blancos ─────────────────────────────────────────────────────────────────
local function findDummies()
    dummies = {}
    for i = 1, num(dummyCount, 8) do
        local o = Brakeza:getObjectByName("Dummy_" .. i)
        if o ~= nil then
            dummies[#dummies + 1] = { obj = o, hp = num(dummyHealth, 3), flash = 0, respawn = 0, name = "Dummy_" .. i }
        end
    end
end

local function updateDummies(dt)
    for _, d in ipairs(dummies) do
        if d.flash > 0 then
            d.flash = d.flash - dt
            if d.flash <= 0 then pcall(function() d.obj:clearHighlight() end) end
        end
        if d.respawn > 0 then
            d.respawn = d.respawn - dt
            if d.respawn <= 0 then
                d.hp = num(dummyHealth, 3)
                pcall(function() d.obj:setEnabled(true) end)
            end
        end
    end
end

-- ¿Ha tocado la bala algún blanco? (distancia en el plano XZ, con el radio según la escala del blanco)
local function hitTest(bullet)
    local bp = bullet.obj:getPosition()
    for _, d in ipairs(dummies) do
        if d.hp > 0 then
            local dp = d.obj:getPosition()
            local dx, dz = bp.x - dp.x, bp.z - dp.z
            local radius = 0.5 * (tonumber(d.obj:getScale()) or 1.0) + 0.2
            if dx * dx + dz * dz < radius * radius and bp.y < dp.y + 3.0 then
                d.hp = d.hp - 1
                d.flash = 0.12
                pcall(function() d.obj:setHighlight(1.0, 0.55, 0.1, 1.0) end)
                if d.hp <= 0 then
                    kills = kills + 1
                    d.respawn = 3.0
                    pcall(function() d.obj:clearHighlight(); d.obj:setEnabled(false) end)
                end
                return true
            end
        end
    end
    return false
end

local function updateBullets(dt)
    local speed = num(bulletSpeed, 30.0)
    local i = 1
    while i <= #bullets do
        local b = bullets[i]
        b.life = b.life - dt
        local gone = b.life <= 0
        if not gone then
            local p = b.obj:getPosition()
            b.obj:setPosition(Vertex3D.new(p.x + b.dir.x * speed * dt, p.y, p.z + b.dir.z * speed * dt))
            gone = hitTest(b)
        end
        if gone then
            pcall(function() b.obj:setRemoved(true) end)
            table.remove(bullets, i)
        else
            i = i + 1
        end
    end
end

-- ── cámara y marca de hacia dónde mira ──────────────────────────────────────
local function updateCamera()
    local p = this:getPosition()
    local height, dist = num(cameraHeight, 14.0), num(cameraDistance, 6.0)
    camera:setPosition(Vertex3D.new(p.x, p.y + height, p.z + dist))
    -- Fixed tilt towards the player (the same 66 degrees the scene camera has). camera:LookAt(this) left the view empty
    camera:setRotationFromEulerAngles(math.deg(math.atan2(height, dist)), 0, 0)
end

local function updateMuzzle()
    if muzzle == nil then return end
    local p = this:getPosition()
    muzzle:setPosition(Vertex3D.new(p.x + facing.x * 0.85, p.y + 0.3, p.z + facing.z * 0.85))
end

-- ── HUD ─────────────────────────────────────────────────────────────────────
local function drawUI()
    local w, h = window:getWidth(), window:getHeight()
    local white, dim = Color.new(1, 1, 1, 1), Color.new(0.6, 0.6, 0.65, 1)

    local ammoColor = Color.new(0.2, 1, 0.3, 1)
    if currentAmmo <= 5 then ammoColor = Color.new(1, 0.5, 0, 1) end
    local ammoText = "AMMO " .. currentAmmo .. "/" .. num(maxAmmo, 30)
    if isReloading then
        ammoText = "RELOADING " .. math.floor((1 - reloadTimer / num(reloadTime, 1.5)) * 100) .. "%"
        ammoColor = Color.new(1, 1, 0, 1)
    end
    textWriter:writeTextTTFAutoSize(20, h - 150, ammoText, ammoColor, 1.1)

    local dashReady = dashCooldownTimer <= 0
    textWriter:writeTextTTFAutoSize(20, h - 118, dashReady and "DASH READY" or string.format("DASH %.1f", dashCooldownTimer),
        dashReady and Color.new(0.2, 0.9, 1, 1) or dim, 0.9)

    local acc = shots > 0 and math.floor(kills / shots * 1000) / 10 or 0
    textWriter:writeTextTTFAutoSize(20, 20, string.format("TARGETS DOWN %d     SHOTS %d", kills, shots), white, 1.0)

    local x = 20
    local y0 = 56
    textWriter:writeTextTTFAutoSize(x, y0, usingPad and "[ GAMEPAD ]" or "[ KEYBOARD + MOUSE ]", Color.new(1, 0.9, 0.2, 1), 0.9)
    if usingPad then
        textWriter:writeTextTTFAutoSize(x, y0 + 26, "Left stick   move", dim, 0.8)
        textWriter:writeTextTTFAutoSize(x, y0 + 50, "Right stick  aim", dim, 0.8)
        textWriter:writeTextTTFAutoSize(x, y0 + 74, "RT / RB      shoot", dim, 0.8)
        textWriter:writeTextTTFAutoSize(x, y0 + 98, "LT / A       dash", dim, 0.8)
        textWriter:writeTextTTFAutoSize(x, y0 + 122, "X            reload", dim, 0.8)
    else
        textWriter:writeTextTTFAutoSize(x, y0 + 26, "W A S D      move", dim, 0.8)
        textWriter:writeTextTTFAutoSize(x, y0 + 50, "Mouse        aim", dim, 0.8)
        textWriter:writeTextTTFAutoSize(x, y0 + 74, "Left button  shoot", dim, 0.8)
        textWriter:writeTextTTFAutoSize(x, y0 + 98, "Right button dash", dim, 0.8)
        textWriter:writeTextTTFAutoSize(x, y0 + 122, "R            reload", dim, 0.8)
    end
    if not input:isGameControllerAvailable() then
        textWriter:writeTextTTFAutoSize(x, y0 + 152, "Plug in a gamepad for twin-stick", Color.new(0.5, 0.5, 0.55, 1), 0.7)
    end
end

local function drawAimLine()
    local render = Components:Render()
    local p = this:getPosition()
    local from = Vertex3D.new(p.x, p.y + 0.3, p.z)
    local len = num(aimLineLength, 8.0)
    local to = Vertex3D.new(from.x + facing.x * len, from.y, from.z + facing.z * len)
    render:DrawLine(from, to, isReloading and Color.new(1, 1, 0, 0.5) or Color.new(1, 0.15, 0.1, 0.9))
end

-- ── ciclo de vida ───────────────────────────────────────────────────────────
function onStart()
    input    = Components:Input()
    camera   = Components:Camera():getCamera()
    window   = Components:Window()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")

    startPosition = this:getPosition()
    currentAmmo = num(maxAmmo, 30)
    fireTimer, dashTimer, dashCooldownTimer, reloadTimer = 0, 0, 0, 0
    isDashing, isReloading, usingPad, mouseAimActive = false, false, false, false
    shots, kills, bulletCounter = 0, 0, 0
    bullets = {}
    facing = Vertex3D.new(0, 0, -1)
    lastMouseX, lastMouseY = -1, -1

    -- El cuerpo no debe girar ni tumbarse por la física: la dirección en la que mira se muestra con la marca y la línea
    this:setAngularFactor(Vertex3D.new(0, 0, 0))
    this:setFriction(0.9)
    this:setRestitution(0)
    this:setLinearDamping(0.5)

    muzzle = ObjectFactory.Mesh3D("../assets/models/basic/cube.fbx", Vertex3D.new(startPosition.x, startPosition.y + 0.3, startPosition.z))
    muzzle:setName("tw_muzzle")
    muzzle:setScale(0.35)
    muzzle:setSelectable(false)

    findDummies()
    Components:Input():setPadEnabled(true)
end

function onUpdate()
    local ok, err = pcall(function()
        local dt = clamp(Brakeza:getDeltaTime(), 0.0001, 0.1)
        local w, h = window:getWidth(), window:getHeight()

        -- Los blancos pueden terminar de cargar después que el jugador
        if #dummies == 0 then findDummies() end

        -- Si se cae del mapa, vuelve al principio
        local p = this:getPosition()
        if p.y < startPosition.y - 15 then
            this:setPosition(Vertex3D.new(startPosition.x, startPosition.y + 1, startPosition.z))
            this:setLinearVelocity(Vertex3D.new(0, 0, 0))
        end

        local basis = screenBasis(w, h)

        updateAim(basis, w, h)
        handleMovement(basis)
        handleDash(basis, dt)
        handleShooting(dt)
        handleReload(dt)

        updateBullets(dt)
        updateDummies(dt)
        updateCamera()
        updateMuzzle()
        drawAimLine()
        drawUI()
    end)
    if not ok then print("[TwinStick] ERROR: " .. tostring(err)) end
end

function onCollision(with)
end

function onEnd()
    for _, b in ipairs(bullets) do pcall(function() b.obj:setRemoved(true) end) end
    bullets = {}
    if muzzle ~= nil then pcall(function() muzzle:setRemoved(true) end) end
    muzzle = nil
end
