-- ProjectileLauncher.lua
-- Controls:
--   A / D  — yaw (horizontal)
--   W / S  — pitch (vertical)
--   Q / E  — decrease / increase power
--   F      — fire

local PROJECTILE_MESH = "../assets/models/basic/icosphere.fbx"
local PROJ_SCALE      = 0.15
local SCALE_STEP      = 1.0
local PROJ_LIFETIME   = 6.0
local ROT_SPEED       = 70.0
local PWR_STEP        = 8.0

-- La cámara empieza mirando al frente (0,0)
-- pitch + = mira arriba / dispara hacia arriba
-- yaw   + = gira a la derecha
local yaw     = 0.0
local pitch   = 0.0
local power   = 18.0
local projSize = 1.0
local counter = 0

local pending = {}
local active  = {}

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
    -- Aplicar orientación inicial a la cámara
    camera:setRotationFromEulerAngles(-pitch, yaw, 0)
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    handleInput(dt)
    promotePending(dt)
    tickActive(dt)
    drawHUD()
end

-- ── input ────────────────────────────────────────────────────────────────────

function handleInput(dt)
    if input:isCharPressed("A") then yaw   = yaw   - ROT_SPEED * dt end
    if input:isCharPressed("D") then yaw   = yaw   + ROT_SPEED * dt end
    if input:isCharPressed("W") then pitch = pitch + ROT_SPEED * dt end
    if input:isCharPressed("S") then pitch = pitch - ROT_SPEED * dt end
    pitch = math.max(-75, math.min(75, pitch))

    if input:isCharPressed("Q") then power = math.max(2.0,   power - PWR_STEP * dt) end
    if input:isCharPressed("E") then power = math.min(120.0, power + PWR_STEP * dt) end

    if input:isCharFirstEventDown("Z") then projSize = math.max(1.0, projSize - SCALE_STEP) end
    if input:isCharFirstEventDown("X") then projSize = math.min(5.0, projSize + SCALE_STEP) end

    -- Rotar la cámara para que la mira FPS coincida con la dirección de disparo
    -- pitch+ = mirar arriba  →  rotX negativo en el motor
    camera:setRotationFromEulerAngles(-pitch, yaw, 0)

    if input:isCharFirstEventDown("F") then spawnProjectile() end
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
                proj:fire(p.dir, p.pow, 98.0, 0.12,
                    CollisionGroups.ProjectileGroup,
                    CollisionGroups.AllFilter)
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
    tw:writeTextTTFAutoSize(20, wh - 28, "A/D: yaw   W/S: pitch   Q/E: power   Z/X: size   F: fire", C_HINT, 0.45)
end
