-- SwarmDemo.lua
-- Demo del Swarm (boids): mueve el depredador (cubo rojo) de varias formas para ver a la bandada huir.
--
-- CONTROLS
--   1 / 2 / 3    movimiento del depredador: ocho / caza al boid más cercano / quieto en el centro
--   Space        pausa / reanuda el depredador
--   Up / Down    velocidad del depredador +- 0.25
--   H            mostrar / ocultar el panel
--
-- Los boids se cargan de forma asíncrona (el enjambre los va resolviendo solo): el panel muestra
-- cuántos hay ya en la escena.

local PREDATOR_NAME = "Predator_1"
local SWARM_NAME    = "SwarmDemo"
local BOID_PREFIX   = "Boid_"
local BOID_COUNT    = 30
local FLEE_RADIUS   = 6.0     -- = predatorThreshold del enjambre en la escena (solo para contar huidas)

local EIGHT_RADIUS, EIGHT_HEIGHT, EIGHT_SPEED = 6.0, 3.0, 0.9
local HUNT_SPEED = 3.2        -- u/s; menos que el paso de pánico de los boids para que puedan escapar
local BOX_MARGIN = 4.5        -- el depredador no sale de esta distancia al centro del enjambre

local MODES = {
    { name = "Figure-eight" },
    { name = "Hunt nearest boid" },
    { name = "Parked in the centre" },
}

local C_TITLE = Color.new(0.20, 0.85, 1.00, 1.0)
local C_WHITE = Color.new(1.00, 1.00, 1.00, 1.0)
local C_DIM   = Color.new(0.55, 0.55, 0.60, 1.0)
local C_ON    = Color.new(0.20, 1.00, 0.30, 1.0)
local C_OFF   = Color.new(1.00, 0.55, 0.20, 1.0)
local C_KEY   = Color.new(1.00, 0.90, 0.20, 1.0)
local C_ALERT = Color.new(1.00, 0.30, 0.25, 1.0)

local S = {
    mode = 1, paused = false, speedMul = 1.0, showHUD = true,
    t = 0.0,
    predator = nil, swarm = nil, boids = {},
    center = { x = 0, y = 5, z = 0 },
    found = 0, fleeing = 0,
}

local input, render, tw

local function alive(o)
    if o == nil then return false end
    local ok, removed = pcall(function() return o:isRemoved() end)
    return ok and not removed
end

-- Los objetos de la escena cargan de forma asíncrona: se resuelven los que falten, sin bloquear
local function resolve()
    if not alive(S.predator) then S.predator = Brakeza:getObjectByName(PREDATOR_NAME) end
    if not alive(S.swarm) then
        S.swarm = Brakeza:getObjectByName(SWARM_NAME)
        if S.swarm ~= nil then
            local p = S.swarm:getPosition()
            S.center = { x = p.x, y = p.y, z = p.z }
        end
    end
    local found = 0
    for i = 1, BOID_COUNT do
        local b = S.boids[i]
        if not alive(b) then
            b = Brakeza:getObjectByName(BOID_PREFIX .. i)
            S.boids[i] = b
        end
        if b ~= nil then found = found + 1 end
    end
    S.found = found
end

local function nearestBoid(px, py, pz)
    local best, bestD = nil, math.huge
    for i = 1, BOID_COUNT do
        local b = S.boids[i]
        if alive(b) then
            local p = b:getPosition()
            local dx, dy, dz = p.x - px, p.y - py, p.z - pz
            local d = dx * dx + dy * dy + dz * dz
            if d < bestD then best, bestD = p, d end
        end
    end
    return best, math.sqrt(bestD)
end

local function clampToBox(x, y, z)
    local c = S.center
    local function cl(v, c0) return math.max(c0 - BOX_MARGIN - 1.5, math.min(c0 + BOX_MARGIN + 1.5, v)) end
    return cl(x, c.x), cl(y, c.y), cl(z, c.z)
end

local function movePredator(dt)
    if not alive(S.predator) or S.paused then return end
    local c = S.center
    local pos = S.predator:getPosition()

    if S.mode == 1 then
        S.t = S.t + dt * EIGHT_SPEED * S.speedMul
        -- Lissajous: x sigue sin(t), z sigue sin(2t)
        S.predator:setPosition(Vertex3D.new(
            c.x + EIGHT_RADIUS * math.sin(S.t),
            c.y + EIGHT_HEIGHT * math.sin(S.t * 0.5),
            c.z + EIGHT_RADIUS * 0.6 * math.sin(S.t * 2)))
    elseif S.mode == 2 then
        local target, dist = nearestBoid(pos.x, pos.y, pos.z)
        if target ~= nil and dist > 0.05 then
            local step = math.min(dist, HUNT_SPEED * S.speedMul * dt)
            local nx = pos.x + (target.x - pos.x) / dist * step
            local ny = pos.y + (target.y - pos.y) / dist * step
            local nz = pos.z + (target.z - pos.z) / dist * step
            nx, ny, nz = clampToBox(nx, ny, nz)
            S.predator:setPosition(Vertex3D.new(nx, ny, nz))
        end
    else
        S.predator:setPosition(Vertex3D.new(c.x, c.y, c.z))
    end
end

local function countFleeing()
    if not alive(S.predator) then S.fleeing = 0; return end
    local pp = S.predator:getPosition()
    local n = 0
    for i = 1, BOID_COUNT do
        local b = S.boids[i]
        if alive(b) then
            local p = b:getPosition()
            local dx, dy, dz = p.x - pp.x, p.y - pp.y, p.z - pp.z
            if dx * dx + dy * dy + dz * dz < FLEE_RADIUS * FLEE_RADIUS then n = n + 1 end
        end
    end
    S.fleeing = n
end

local function drawHUD()
    if not S.showHUD then
        tw:writeTextAtlasCache(14, 14, "H - show panel", C_DIM, 0.52)
        return
    end
    tw:writeTextAtlasCache(14, 40, "SWARM / BOIDS", C_TITLE, 0.70)
    tw:writeTextAtlas(14, 80, "Predator: " .. MODES[S.mode].name, C_WHITE, 0.62)
    tw:writeTextAtlas(14, 106, S.paused and "PAUSED" or "MOVING", S.paused and C_OFF or C_ON, 0.58)
    tw:writeTextAtlas(14, 132, string.format("Speed x%.2f", S.speedMul), C_DIM, 0.55)

    local loaded = S.found == BOID_COUNT and S.predator ~= nil
    tw:writeTextAtlas(14, 164, string.format("Boids in scene: %d / %d%s", S.found, BOID_COUNT,
        loaded and "" or "  (loading...)"), loaded and C_ON or C_OFF, 0.55)
    tw:writeTextAtlas(14, 190, string.format("Fleeing: %d", S.fleeing), S.fleeing > 0 and C_ALERT or C_DIM, 0.62)

    local y = 230
    local keys = {
        { "1 / 2 / 3", "figure-eight / hunt / parked" },
        { "Space",     "pause predator" },
        { "Up/Down",   "predator speed" },
        { "H",         "hide panel" },
    }
    for _, k in ipairs(keys) do
        tw:writeTextAtlasCache(14, y, k[1], C_KEY, 0.55)
        tw:writeTextAtlasCache(150, y, k[2], C_DIM, 0.55)
        y = y + 24
    end
end

function onStart()
    input  = Components:Input()
    render = Components:Render()
    tw     = render:getTextWriter()
    S.mode, S.paused, S.speedMul, S.showHUD, S.t = 1, false, 1.0, true, 0.0
    S.predator, S.swarm, S.boids, S.found, S.fleeing = nil, nil, {}, 0, 0
    print("[SwarmDemo] 1/2/3 = predator mode, Space = pause, Up/Down = speed, H = panel")
end

local function _update()
    input  = Components:Input()
    render = Components:Render()
    tw     = render:getTextWriter()
    local dt = Brakeza:getDeltaTime()

    resolve()

    for i = 1, #MODES do
        if input:isCharFirstEventDown(tostring(i)) then S.mode = i; S.t = 0.0 end
    end
    if input:isCharFirstEventDown("Space") then S.paused = not S.paused end
    if input:isCharFirstEventDown("Up")    then S.speedMul = math.min(3.0,  S.speedMul + 0.25) end
    if input:isCharFirstEventDown("Down")  then S.speedMul = math.max(0.25, S.speedMul - 0.25) end
    if input:isCharFirstEventDown("H")     then S.showHUD = not S.showHUD end

    movePredator(dt)
    countFleeing()
    drawHUD()
end

function onUpdate()
    local ok, err = pcall(_update)
    if not ok then print("[SwarmDemo] ERROR: " .. tostring(err)) end
end

function onEnd()
    S.predator, S.swarm, S.boids = nil, nil, {}
end
