-- PerformanceTest.lua
-- Stress-test script for Brakeza3D
--
-- CONTROLS
--   Space       toggle continuous spawning
--   Delete      spawn one batch immediately
--   R           reset — remove all spawned objects
--   H           toggle HUD visibility
--   1-5         switch mode
--   Up / Down   batch size ±1
--   [ / ]       batch size ±10
--   Left/Right  slower / faster spawn rate

tw     = nil
input  = nil
win    = nil
render = nil

-- ── runtime state ────────────────────────────────────────────────────────────
local spawned       = {}
local spawnCount    = 0
local spawning      = false
local spawnTimer    = 0.0
local shadowCasting = false   -- S toggles shadow pass participation
local mode        = 1
local batchSize   = 5
local spawnRate   = 0.15
local maxObjects  = 500
local loadTimer   = 0.0   -- elapsed since spawning started
local loadTime    = nil   -- final time when cap was reached (nil = not finished yet)
local loadRunning = false -- true while measuring
local showHUD     = true  -- toggle with H
local spawnArea   = 20.0  -- half-side of the spawn square (world units)
local spawnY      = 2.0   -- spawn height

-- ── mode descriptors ─────────────────────────────────────────────────────────
local MODES = {
    { name = "1 - Static Meshes",     hint = "pure draw calls",     r=0.30, g=1.00, b=0.30 },
    { name = "2 - Physics Meshes",    hint = "physics + rendering", r=1.00, g=0.60, b=0.10 },
    { name = "3 - Particle Emitters", hint = "GPU particles",       r=0.40, g=0.55, b=1.00 },
    { name = "4 - Point Lights",      hint = "deferred light pass", r=1.00, g=1.00, b=0.20 },
    { name = "5 - Animated Meshes",   hint = "skinned mesh draw",   r=0.90, g=0.30, b=0.90 },
}

local MODELS = {
    "../assets/models/basic/cube.fbx",
    "../assets/models/basic/icosphere.fbx",
    "../assets/models/basic/cilinder.fbx",
    "../assets/models/basic/torus.fbx",
    "../assets/models/basic/cone.fbx",
}

-- ── colours ──────────────────────────────────────────────────────────────────
local C_TITLE    = Color.new(0.20, 0.85, 1.00, 1.0)
local C_WHITE    = Color.new(1.00, 1.00, 1.00, 1.0)
local C_DIM      = Color.new(0.45, 0.45, 0.48, 1.0)
local C_ON       = Color.new(0.20, 1.00, 0.30, 1.0)
local C_OFF      = Color.new(1.00, 0.28, 0.28, 1.0)
local C_COUNT    = Color.new(1.00, 0.85, 0.00, 1.0)
local C_FPS_GOOD = Color.new(0.20, 1.00, 0.30, 1.0)
local C_FPS_MED  = Color.new(1.00, 0.80, 0.00, 1.0)
local C_FPS_BAD  = Color.new(1.00, 0.20, 0.20, 1.0)
local C_KEY      = Color.new(1.00, 0.90, 0.20, 1.0)
local C_DESC     = Color.new(0.85, 0.85, 0.85, 1.0)
local C_HEAD     = Color.new(0.55, 0.55, 0.60, 1.0)

-- ── layout constants ─────────────────────────────────────────────────────────
local KW      = 460
local KH      = 540
local ROW     = 28
local KS      = 0.58
local DS      = 0.56
-- mode list height: 5 modes × 24 = 120
local STATS_Y = 60 + 120 + 20  -- 200


-- ── helpers ──────────────────────────────────────────────────────────────────
local function rnd(a, b) return a + math.random() * (b - a) end

local function applyFlags(obj)
    obj:setShadowMap(shadowCasting)
end

local function nextModel(i)
    return MODELS[((i - 1) % #MODELS) + 1]
end

local function spawnPos()
    return Vertex3D.new(
        rnd(-spawnArea, spawnArea),
        spawnY,
        rnd(-spawnArea, spawnArea)
    )
end

-- ── spawners (one per mode) ───────────────────────────────────────────────────
local function spawnStaticMesh()
    spawnCount = spawnCount + 1
    local name = "PT_" .. spawnCount
    local obj  = ObjectFactory.Mesh3D(nextModel(spawnCount), spawnPos())
    obj:setName(name)
    applyFlags(obj)
    table.insert(spawned, name)
end

local function spawnPhysicsMesh()
    spawnCount = spawnCount + 1
    local name = "PT_" .. spawnCount
    local obj  = ObjectFactory.Mesh3D(nextModel(spawnCount), spawnPos())
    obj:setName(name)
    applyFlags(obj)
    obj:setCollisionsEnabled(true)
    obj:setMass(1.0)
    obj:setLinearDamping(0.05)
    obj:setAngularDamping(0.10)
    obj:SetupRigidBodyCollider(CollisionShape.TRIANGLE_MESH_SHAPE)
    table.insert(spawned, name)
end

local function spawnParticleEmitter()
    spawnCount = spawnCount + 1
    local name = "PT_" .. spawnCount
    local cf   = Color.new(rnd(0.3,1), rnd(0.3,1), rnd(0.3,1), 1)
    local ct   = Color.new(rnd(0,0.4), rnd(0,0.4), rnd(0,0.4), 0)
    local em   = ObjectFactory.ParticleEmitter(spawnPos(), cf, ct)
    em:setName(name)
    em:setBelongToScene(false)
    local ctx = ParticlesContext.new()
    ctx.PARTICLES_BY_SECOND  = 80
    ctx.PARTICLE_LIFESPAN    = 1.5
    ctx.MIN_VELOCITY         = 1.5
    ctx.MAX_VELOCITY         = 4.0
    ctx.GRAVITY              = -2.0
    ctx.SMOKE_ANGLE_RANGE    = 45
    ctx.MIN_ALPHA            = 0.5
    ctx.MAX_ALPHA            = 1.0
    ctx.DECELERATION_FACTOR  = 0.97
    em:setContext(ctx)
    Brakeza:AddObject3D(em, name)
    table.insert(spawned, name)
end

local function spawnLight()
    if #spawned >= 60 then return end  -- engine cap: 64 point lights
    spawnCount = spawnCount + 1
    local name = "PT_" .. spawnCount
    local col  = Color.new(rnd(0.3,1), rnd(0.3,1), rnd(0.3,1), 1)
    local amb  = Color.new(0.02, 0.02, 0.02, 1)
    local obj  = ObjectFactory.LightPoint(spawnPos(), amb, col, col)
    obj:setName(name)
    obj:setBelongToScene(false)
    Brakeza:AddObject3D(obj, name)
    table.insert(spawned, name)
end

local function spawnAnimatedMesh()
    spawnCount = spawnCount + 1
    local name = "PT_" .. spawnCount
    local obj  = ObjectFactory.Mesh3DAnimation(nextModel(spawnCount), spawnPos())
    obj:setName(name)
    applyFlags(obj)
    table.insert(spawned, name)
end

local SPAWNERS = {
    spawnStaticMesh,
    spawnPhysicsMesh,
    spawnParticleEmitter,
    spawnLight,
    spawnAnimatedMesh,
}

-- ── batch & reset ─────────────────────────────────────────────────────────────
local function spawnBatch()
    local cap = (mode == 4) and math.min(maxObjects, 60) or maxObjects
    local n   = math.min(batchSize, cap - #spawned)
    if n <= 0 then return end
    for i = 1, n do
        SPAWNERS[mode]()
    end
end

local function doReset()
    for _, name in ipairs(spawned) do
        local obj = Brakeza:getObjectByName(name)
        if obj then obj:setRemoved(true) end
    end
    spawned     = {}
    spawnCount  = 0
    spawning    = false
    spawnTimer  = 0.0
    loadTimer   = 0.0
    loadTime    = nil
    loadRunning = false
    print("[PerfTest] reset — all spawned objects removed")
end

-- ── HUD draw ──────────────────────────────────────────────────────────────────
local function fpsColor(fps)
    if fps >= 50 then return C_FPS_GOOD
    elseif fps >= 30 then return C_FPS_MED
    else return C_FPS_BAD end
end

function drawHUD()
    local sw = win:getWidth();  if sw == 0 then sw = 1280 end
    local sh = win:getHeight(); if sh == 0 then sh = 720  end
    local kx = sw - KW - 220
    local ky = sh - KH - 16

    if showHUD then
        -- left panel
        tw:writeTextAtlasCache(14, 40, "PERFORMANCE TEST", C_TITLE, 0.70)

        local y = 86
        for i, m in ipairs(MODES) do
            local sel = (i == mode)
            local col = sel and Color.new(m.r, m.g, m.b, 1) or C_DIM
            if sel then tw:writeTextAtlasCache(6, y, ">", C_WHITE, 0.58) end
            tw:writeTextAtlasCache(20, y, m.name, col, 0.58)
            if sel then
                tw:writeTextAtlasCache(200, y, m.hint, Color.new(m.r * 0.65, m.g * 0.65, m.b * 0.65, 1), 0.52)
            end
            y = y + 24
        end

        local sy = STATS_Y + 26
        local capLabel = (mode == 4) and "60 (light cap)" or tostring(maxObjects)
        tw:writeTextAtlasCache(14, sy,       "Objects : " .. #spawned .. " / " .. capLabel,                               C_COUNT, 0.62)
        tw:writeTextAtlasCache(14, sy + 28,  "Batch   : " .. batchSize,                                                   C_WHITE, 0.58)
        tw:writeTextAtlasCache(14, sy + 56,  "Rate    : " .. string.format("%.2f", spawnRate) .. "s",                     C_WHITE, 0.58)
        tw:writeTextAtlasCache(14, sy + 84,  "Max     : " .. maxObjects,                                                  C_COUNT, 0.58)
        tw:writeTextAtlasCache(14, sy + 112, "Area    : " .. math.floor(spawnArea*2) .. " x " .. math.floor(spawnArea*2), C_DIM,   0.55)
        tw:writeTextAtlas(14, sy + 140, "Shadows : " .. (shadowCasting and "ON" or "OFF"), shadowCasting and C_ON or C_OFF, 0.58)

        -- load timer (changes every frame while running — immediate)
        local timerLabel, timerCol
        if loadRunning then
            timerLabel = "Load    : " .. string.format("%.1f", loadTimer) .. "s  (running...)"
            timerCol   = C_FPS_MED
        elseif loadTime then
            timerLabel = "Load    : " .. string.format("%.2f", loadTime) .. "s  (done)"
            timerCol   = C_FPS_GOOD
        else
            timerLabel = "Load    : --"
            timerCol   = C_DIM
        end
        tw:writeTextAtlas(14, sy + 168, timerLabel, timerCol, 0.58)

        -- FPS changes every frame — immediate
        local fps = render:getFps()
        tw:writeTextAtlas(14, sy + 200, "FPS: " .. fps, fpsColor(fps), 0.82)

        -- controls panel — key labels always static, desc static except rows with live values
        local function krow(row, key, desc, kc, dc, dynDesc)
            local ry = ky + 46 + row * ROW
            tw:writeTextAtlasCache(kx + 14,  ry, key,  kc or C_KEY,  KS)
            if dynDesc then
                tw:writeTextAtlas(kx + 148, ry, desc, dc or C_DESC, DS)
            else
                tw:writeTextAtlasCache(kx + 148, ry, desc, dc or C_DESC, DS)
            end
        end
        tw:writeTextAtlasCache(kx + 14,  ky + 14, "CONTROLS", C_TITLE, 0.64)
        tw:writeTextAtlasCache(kx + 148, ky + 14, "KEY",      C_HEAD,  0.54)
        tw:writeTextAtlasCache(kx + 260, ky + 14, "ACTION",   C_HEAD,  0.54)
        krow(0,  "Space",        spawning and "STOP spawning" or "START spawning",
                Color.new(1,1,1,1), spawning and C_ON or C_OFF, true)
        krow(1,  "Delete",       "spawn one batch now")
        krow(2,  "R",            "reset — remove all objects", Color.new(1.0, 0.4, 0.4, 1))
        krow(3,  "H",            "toggle HUD", C_KEY, C_DIM)
        krow(4,  "S",            shadowCasting and "shadows ON (new objects)" or "shadows OFF (new objects)",
                C_KEY, shadowCasting and C_ON or C_OFF, true)
        krow(5,  "1",            "mode: Static Meshes")
        krow(6,  "2",            "mode: Physics Meshes")
        krow(7,  "3",            "mode: Particle Emitters")
        krow(8,  "4",            "mode: Point Lights")
        krow(9,  "5",            "mode: Animated Meshes")
        krow(10, "Up/Down",      "batch size")
        krow(11, "Left / Right", "spawn rate")
        krow(12, "N / M",        "max objects")
    else
        -- minimal indicator when HUD is hidden
        tw:writeTextAtlasCache(14, 14, "H — show HUD", C_DIM, 0.52)
    end
end

-- ── lifecycle ─────────────────────────────────────────────────────────────────
function onStart()
    input  = Components:Input()
    win    = Components:Window()
    render = Components:Render()
    tw     = render:getTextWriter()
    math.randomseed(42)
    spawned      = {}
    spawnCount   = 0
    spawning     = false
    spawnTimer   = 0.0

    mode         = 1
    batchSize    = 5
    spawnRate    = 0.15
    showHUD      = true
    print("[PerfTest] ════════════════════════════════════")
    print("[PerfTest]  PERFORMANCE TEST — CONTROLS")
    print("[PerfTest] ════════════════════════════════════")
    print("[PerfTest]  Space       — toggle continuous spawn")
    print("[PerfTest]  Delete      — spawn one batch now")
    print("[PerfTest]  R           — reset / remove all objects")
    print("[PerfTest]  1           — mode: Static Meshes")
    print("[PerfTest]  2           — mode: Physics Meshes")
    print("[PerfTest]  3           — mode: Particle Emitters")
    print("[PerfTest]  4           — mode: Point Lights (cap 60)")
    print("[PerfTest]  5           — mode: Animated Meshes")
    print("[PerfTest]  Up / Down   — increase / decrease batch size")
    print("[PerfTest]  Left/Right  — slower / faster spawn rate")
    print("[PerfTest]  S           — toggle shadow casting (default OFF)")
    print("[PerfTest] ════════════════════════════════════")
end

local function _update()
    input  = Components:Input()
    win    = Components:Window()
    render = Components:Render()
    tw     = render:getTextWriter()

    local dt = Brakeza:getDeltaTime()

    -- mode select
    for i = 1, 5 do
        if input:isCharFirstEventDown(tostring(i)) and mode ~= i then
            mode = i
            print("[PerfTest] mode: " .. MODES[i].name)
        end
    end

    -- toggle spawn
    if input:isCharFirstEventDown("Space") then
        spawning = not spawning
        if spawning then
            loadTimer   = 0.0
            loadTime    = nil
            loadRunning = true
        end
        print("[PerfTest] spawning: " .. tostring(spawning))
    end

    -- spawn one batch
    if input:isCharFirstEventDown("Delete") then
        spawnBatch()
    end

    -- reset
    if input:isCharFirstEventDown("R") then
        doReset()
    end

    -- toggle HUD
    if input:isCharFirstEventDown("H") then
        showHUD = not showHUD
    end

    -- toggle shadow casting for newly spawned objects
    if input:isCharFirstEventDown("S") then
        shadowCasting = not shadowCasting
        print("[PerfTest] shadowCasting: " .. tostring(shadowCasting))
    end

    -- batch size  (Up/Down ±1, [ / ] ±10)
    if input:isCharFirstEventDown("Up") then
        batchSize = math.min(batchSize + 5, 500)
        print("[PerfTest] batch=" .. batchSize)
    end
    if input:isCharFirstEventDown("Down") then
        batchSize = math.max(batchSize - 5, 1)
        print("[PerfTest] batch=" .. batchSize)
    end
    if input:isCharFirstEventDown("]") then
        batchSize = math.min(batchSize + 10, 500)
        print("[PerfTest] batch=" .. batchSize)
    end
    if input:isCharFirstEventDown("[") then
        batchSize = math.max(batchSize - 10, 1)
        print("[PerfTest] batch=" .. batchSize)
    end

    -- spawn rate
    if input:isCharFirstEventDown("Right") then
        spawnRate = math.max(spawnRate - 0.05, 0.02)
    end
    if input:isCharFirstEventDown("Left") then
        spawnRate = math.min(spawnRate + 0.05, 2.0)
    end

    -- max objects cap
    if input:isCharFirstEventDown("M") then
        maxObjects = math.min(maxObjects + 50, 2000)
        print("[PerfTest] maxObjects=" .. maxObjects)
    end
    if input:isCharFirstEventDown("N") then
        maxObjects = math.max(maxObjects - 50, 50)
        print("[PerfTest] maxObjects=" .. maxObjects)
    end

    -- accumulate load timer
    if loadRunning then
        loadTimer = loadTimer + dt
    end

    -- continuous spawn
    if spawning then
        local cap = (mode == 4) and 60 or maxObjects
        if #spawned >= cap then
            spawning    = false
            loadRunning = false
            loadTime    = loadTimer
            print("[PerfTest] cap reached (" .. #spawned .. " objects) in " .. string.format("%.2f", loadTime) .. "s")
        else
            spawnTimer = spawnTimer + dt
            if spawnTimer >= spawnRate then
                spawnTimer = 0
                spawnBatch()
            end
        end
    end

    drawHUD()
end

function onUpdate()
    local ok, err = pcall(_update)
    if not ok then
        print("[PerfTest] ERROR: " .. tostring(err))
    end
end

function onEnd()
    doReset()
end
