-- ProjectileLauncher.lua
-- Controls:
--   A / D  — yaw (horizontal)
--   W / S  — pitch (vertical)
--   Q / E  — decrease / increase power
--   F      — fire
--   P      — physics debug view on / off
--   Mouse: hold the right button and move to aim (the cursor is captured while you do),
--          wheel = power, left click = fire

local PROJECTILE_MESH = "../assets/models/basic/icosphere.fbx"
local PROJ_SCALE      = 0.30
local SCALE_STEP      = 1.0
local PROJ_LIFETIME   = 6.0
local ROT_SPEED       = 50.0    -- grados/s con la tecla mantenida
local AIM_RESPONSE    = 7.0     -- suavizado del giro con teclas (mayor = mas brusco)
local PWR_STEP        = 8.0
local MOUSE_SENS      = 0.12    -- grados por pixel al apuntar con el ratón
local MOUSE_MAX_STEP  = 80      -- tope de movimiento por frame (evita saltos al capturar el ratón)
local WHEEL_POWER     = 3.0     -- potencia por cada muesca de la rueda
local BROWSER_BAR_PX  = 70      -- franja inferior del DemoBrowser: un clic ahí no dispara

-- La cámara empieza mirando al frente (0,0)
-- pitch + = mira arriba / dispara hacia arriba
-- yaw   + = gira a la derecha
local yaw     = 0.0
local pitch   = -12.0   -- starts looking down at the base (like the scene camera), so the shots land on it and bounce
local power   = 18.0
local projSize = 1.0
local counter = 0
local looking  = false      -- botón derecho pulsado: el ratón apunta
local prevLeft = false      -- para detectar el flanco del clic izquierdo

local pending      = {}
local active       = {}
local activeTrails = {}

local C_TITLE  = Color.new(1.0,  0.86, 0.31, 1.0)
local C_VALUE  = Color.new(0.70, 1.0,  0.70, 1.0)
local C_COUNT  = Color.new(0.63, 0.78, 1.0,  1.0)
local C_HINT   = Color.new(0.55, 0.55, 0.55, 0.85)
local C_CROSS  = Color.new(1.0,  1.0,  1.0,  0.9)
local C_DOT    = Color.new(0.0,  1.0,  0.4,  1.0)

local input  = nil
local render = nil
local tw     = nil
local camera = nil

-- ── lifecycle ────────────────────────────────────────────────────────────────

function onStart()
    input  = Components:Input()
    render = Components:Render()
    tw     = render:getTextWriter()
    camera = Components:Camera():getCamera()
    physicsDebug = false
    -- Arrastrar con el botón izquierdo mira/apunta: el recuadro de selección del editor sobraría
    pcall(function() Components:Render():setDragSelectEnabled(false) end)
    -- Aplicar orientación inicial a la cámara
    camera:setRotationFromEulerAngles(-pitch, yaw, 0)
end

function onEnd()
    pcall(function() Components:Render():setDragSelectEnabled(true) end)   -- devolver la selección con recuadro
    -- no dejar el ratón capturado al salir de la demo
    if looking then
        pcall(function() input:setRelativeMouseMode(false) end)
        looking = false
    end
end

function onUpdate()
    -- A long frame (loading a projectile, a hitch) must not turn a short key press into a huge turn
    local dt = math.min(Brakeza:getDeltaTime(), 0.05)
    handleInput(dt)
    promotePending(dt)
    tickActive(dt)
    drawHUD()
end

-- ── input ────────────────────────────────────────────────────────────────────

-- The keys do not turn the launcher at a constant speed: the turning speed eases towards the wanted one (AIM_RESPONSE),
-- so it starts and stops smoothly, like a camera operator's hand, instead of jumping from 0 to full speed.
local yawVel, pitchVel = 0.0, 0.0
local physicsDebug = false

function handleInput(dt)
    local wantYaw, wantPitch = 0.0, 0.0
    if input:isCharPressed("A") then wantYaw   = wantYaw   - ROT_SPEED end
    if input:isCharPressed("D") then wantYaw   = wantYaw   + ROT_SPEED end
    if input:isCharPressed("W") then wantPitch = wantPitch + ROT_SPEED end
    if input:isCharPressed("S") then wantPitch = wantPitch - ROT_SPEED end
    local k = math.min(1.0, AIM_RESPONSE * dt)
    yawVel   = yawVel   + (wantYaw   - yawVel)   * k
    pitchVel = pitchVel + (wantPitch - pitchVel) * k
    yaw   = yaw   + yawVel   * dt
    pitch = pitch + pitchVel * dt
    handleMouse()
    pitch = math.max(-75, math.min(75, pitch))

    if input:isCharPressed("Q") then power = math.max(2.0,   power - PWR_STEP * dt) end
    if input:isCharPressed("E") then power = math.min(120.0, power + PWR_STEP * dt) end

    if input:isCharFirstEventDown("Z") then projSize = math.max(1.0, projSize - SCALE_STEP) end
    if input:isCharFirstEventDown("X") then projSize = math.min(5.0, projSize + SCALE_STEP) end

    -- Rotar la cámara para que la mira FPS coincida con la dirección de disparo
    -- pitch+ = mirar arriba  →  rotX negativo en el motor
    camera:setRotationFromEulerAngles(-pitch, yaw, 0)

    if input:isCharFirstEventDown("F") then spawnProjectile() end

    -- P: draw / hide the physics shapes (Bullet debug view) to see the real colliders and the contact points of the bounces
    if input:isCharFirstEventDown("P") then
        physicsDebug = not physicsDebug
        Components:Collisions():setEnableDebugMode(physicsDebug)
    end
end

-- Ratón: botón derecho mantenido = apuntar (captura el cursor), rueda = potencia, clic izquierdo = disparar
function handleMouse()
    local right = input:isRightMouseButtonPressed()
    if right ~= looking then
        looking = right
        pcall(function() input:setRelativeMouseMode(looking) end)
    end

    if looking and input:isMouseMotion() then
        local dx = math.max(-MOUSE_MAX_STEP, math.min(MOUSE_MAX_STEP, input:getMouseMotionXRel()))
        local dy = math.max(-MOUSE_MAX_STEP, math.min(MOUSE_MAX_STEP, input:getMouseMotionYRel()))
        yaw   = yaw   + dx * MOUSE_SENS
        pitch = pitch - dy * MOUSE_SENS          -- ratón arriba = mirar arriba
    end

    local wheel = input:getMouseWheelY()
    if wheel ~= 0 then
        power = math.max(2.0, math.min(120.0, power + wheel * WHEEL_POWER))
    end

    -- Disparo con el clic izquierdo (solo en el flanco de bajada). No dispara si el clic fue para la interfaz
    -- del DemoBrowser (la barra inferior y sus paneles) ni mientras se apunta con el botón derecho.
    local left = input:isLeftMouseButtonPressed()
    if left and not prevLeft and not looking and not input:isLeftClickConsumed() then
        local wh = Components:Window():getHeightRender()
        local overBrowserBar = input:getRelativeRendererMouseY() > wh - BROWSER_BAR_PX
        if not overBrowserBar then spawnProjectile() end
    end
    prevLeft = left
end

-- ── spawn ────────────────────────────────────────────────────────────────────

function spawnProjectile()
    counter = counter + 1
    local name = "Proj_" .. counter

    local camPos = camera:getPosition()
    local origin = Vertex3D.new(camPos.x, camPos.y, camPos.z)

    local p = ObjectFactory.Projectile(PROJECTILE_MESH, origin)
    p:setName(name)
    p:setScale(PROJ_SCALE * projSize)

    local yr  = math.rad(yaw)
    local pr  = math.rad(pitch)
    local dir = Vertex3D.new(
        math.sin(yr) * math.cos(pr),
        math.sin(pr),
        -math.cos(yr) * math.cos(pr)
    )

    table.insert(pending, { name = name, obj = p, dir = dir, pow = power, wait = 0 })
    print("[Launcher] queued " .. name)
end

-- ── promote pending → active when mesh loaded ─────────────────────────────────

function promotePending(dt)
    local still = {}
    for _, p in ipairs(pending) do
        p.wait = p.wait + dt
        local inScene = Brakeza:getObjectByName(p.name)
        if inScene ~= nil then
            local proj = Brakeza:getProjectileByName(p.name)
            if proj ~= nil then
                -- Bouncy projectile: Bullet multiplies the restitution of both bodies, so the base (the "target" mesh) is bouncy too
                proj:setRestitution(0.75)
                proj:setFriction(0.5)
                proj:fire(p.dir, p.pow, 98.0, PROJ_SCALE * projSize * 0.8,
                    CollisionGroups.ProjectileGroup,
                    CollisionGroups.AllFilter)

                -- Comet trail: glowing embers that fade from orange to red (it used to be a grey smoke puff)
                local emitter = ObjectFactory.ParticleEmitter(proj:getPosition(), Color.new(1.0, 0.70, 0.20, 1.0), Color.new(1.0, 0.10, 0.0, 0.0))
                if emitter ~= nil then
                    emitter:setTexture("../assets/images/Particles/fire_01.png")
                    emitter:setFollowTarget(proj)
                    local ctx = emitter:getContext()
                    ctx.PARTICLES_BY_SECOND  = 120   -- partículas/s
                    ctx.PARTICLE_LIFESPAN    = 0.8
                    ctx.GRAVITY              = 0.6   -- las brasas suben un poco
                    ctx.SMOKE_ANGLE_RANGE    = 180
                    ctx.MIN_VELOCITY         = 0.5
                    ctx.MAX_VELOCITY         = 2.5
                    ctx.MIN_ALPHA            = 160
                    ctx.MAX_ALPHA            = 255
                    ctx.POSITION_NOISE       = 0.6
                    ctx.VELOCITY_NOISE       = 1.0
                    ctx.DECELERATION_FACTOR  = 0.92
                    ctx.PARTICLE_SIZE_SCALE  = 1.2
                    table.insert(activeTrails, { proj = proj, emitter = emitter })
                end

                table.insert(active, { name = p.name, timer = 0 })
            else
                local scaled = Vertex3D.new(
                    p.dir.x * p.pow,
                    p.dir.y * p.pow,
                    p.dir.z * p.pow
                )
                inScene:ApplyCentralImpulse(scaled)
                table.insert(active, { name = p.name, timer = 0 })
            end
        elseif p.wait < 5.0 then
            table.insert(still, p)
        else
            print("[Launcher] TIMEOUT waiting for " .. p.name)
        end
    end
    pending = still
end

-- ── cleanup old projectiles ───────────────────────────────────────────────────

function tickActive(dt)
    local keep = {}
    for _, a in ipairs(active) do
        a.timer = a.timer + dt
        if a.timer < PROJ_LIFETIME then
            table.insert(keep, a)
        else
            local obj = Brakeza:getObjectByName(a.name)
            if obj ~= nil then obj:setRemoved(true) end
        end
    end
    active = keep

    local aliveTrails = {}
    for _, t in ipairs(activeTrails) do
        if t.proj:isRemoved() then
            t.emitter:detach(1.0)
        else
            table.insert(aliveTrails, t)
        end
    end
    activeTrails = aliveTrails
end

-- ── HUD ──────────────────────────────────────────────────────────────────────

function drawHUD()
    local ww = Components:Window():getWidthRender()
    local wh = Components:Window():getHeightRender()
    local cx = math.floor(ww * 0.5)
    local cy = math.floor(wh * 0.5)

    -- Mira FPS
    local gap = 8
    local len = 18
    local w   = 1.5

    render:DrawLine2D(cx - gap - len, cy, cx - gap,       cy, C_CROSS, w)
    render:DrawLine2D(cx + gap,       cy, cx + gap + len, cy, C_CROSS, w)
    render:DrawLine2D(cx, cy - gap - len, cx, cy - gap,       C_CROSS, w)
    render:DrawLine2D(cx, cy + gap,       cx, cy + gap + len, C_CROSS, w)

    -- Punto central
    render:DrawLine2D(cx - 1, cy, cx + 1, cy, C_DOT, 3.0)
    render:DrawLine2D(cx, cy - 1, cx, cy + 1, C_DOT, 3.0)

    -- Info
    tw:writeTextTTFAutoSize(20, 18,  "=== Projectile Launcher ===",                               C_TITLE, 0.70)
    tw:writeTextTTFAutoSize(20, 46,  string.format("Yaw:   %6.1f deg", yaw),                      C_VALUE, 0.55)
    tw:writeTextTTFAutoSize(20, 66,  string.format("Pitch: %6.1f deg", pitch),                    C_VALUE, 0.55)
    tw:writeTextTTFAutoSize(20, 86,  string.format("Power: %6.1f",     power),                    C_VALUE, 0.55)
    tw:writeTextTTFAutoSize(20, 106, string.format("Size:  %6.1f",     projSize),                 C_VALUE, 0.55)
    tw:writeTextTTFAutoSize(20, 130, string.format("Active: %d  Pending: %d", #active, #pending), C_COUNT, 0.50)
    -- 90 px desde abajo: por encima de la barra del DemoBrowser (56 px) del proyecto de tutoriales
    tw:writeTextTTFAutoSize(20, wh - 90, "A/D W/S or mouse (hold right button): aim   Q/E or wheel: power   Z/X: size   F or left click: fire", C_HINT, 0.45)
end
