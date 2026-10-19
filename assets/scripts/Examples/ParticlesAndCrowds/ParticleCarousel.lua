-- ParticleCarousel.lua
-- Demo de partículas: coloca los emisores de ParticleTest.json en un círculo y lo hace girar
-- delante de la cámara; el efecto "al frente" es el que se está mostrando.
--
-- Para que cada efecto se vea bien, el script cuida tres cosas (las tres se pueden apagar con M):
--   * el FONDO: cada efecto tiene el suyo (fuego sobre casi negro, humo sobre gris azulado, agua sobre azul...)
--     y la transición entre ellos es suave: window:setClearColor(r, g, b, a)
--   * el ZOOM: los efectos pequeños (luciérnagas, chispas) se acercan y los grandes (tornado, humo) se alejan
--   * las RÁFAGAS: relámpagos y fuegos artificiales se encienden y apagan a ratos con emitter:setStopAdd(...)
--
-- CONTROLS
--   Space        play / pause del carrusel (en pausa no avanza solo)
--   Left / Right efecto anterior / siguiente
--   Up / Down    tiempo por efecto +- 1 s
--   E            activar / desactivar la emisión de todos los emisores
--   M            vista automática (fondo + zoom por efecto) on / off
--   Mouse wheel  zoom propio (se suma al automático)
--   H            mostrar / ocultar el panel

-- Variables del script (ParticleCarousel.json); estos valores son por si faltan
-- radius, centerX, centerZ, dwellTime, glideTime

-- Cada entrada = emisor + su peana (nombres de la escena)
local EFFECTS = {
    "01_Campfire", "02_SmokeColumn", "03_Embers", "04_WeldingSparks", "05_Fountain",
    "06_SteamVent", "07_Snow", "08_Rain", "09_MagicSwirl", "10_Fireflies",
    "11_ToxicGas", "12_JetThruster", "13_LavaGeyser_GPU", "14_DustKickup", "15_HealingAura",
    "16_RTS_SmokeBomb", "17_Helix", "18_Tornado", "19_SpiralGalaxy", "20_Sprinkler_GPU",
    "21_SphereBurst", "22_TorusRing", "23_ConeSpray", "24_MeshAura",
    "25_Portal", "26_Lightning", "27_Bubbles", "28_Fireworks", "29_BlackHole", "30_Confetti",
}

-- Cómo se ve mejor cada efecto: fondo (r,g,b), zoom (unidades que la cámara se acerca; negativo = se aleja),
-- ráfaga opcional (on = segundos encendido, period = cada cuántos segundos) y una línea de descripción.
local DEFAULT_BG = { 0.03, 0.04, 0.07 }
local MOODS = {
    ["01_Campfire"]        = { bg = { 0.04, 0.03, 0.04 }, zoom = 2,  desc = "Flame sprites that shrink as they rise, with a flickering light attached" },
    ["02_SmokeColumn"]     = { bg = { 0.10, 0.11, 0.14 }, zoom = -3, desc = "Soft smoke that grows while it rises" },
    ["03_Embers"]          = { bg = { 0.03, 0.02, 0.03 }, zoom = 6,  desc = "Slow sparks drifting up with a random wobble" },
    ["04_WeldingSparks"]   = { bg = { 0.02, 0.02, 0.04 }, zoom = 7,  desc = "Fast sparks thrown upwards and pulled back by gravity" },
    ["05_Fountain"]        = { bg = { 0.02, 0.05, 0.10 }, zoom = 1,  desc = "A narrow jet falling back under gravity" },
    ["06_SteamVent"]       = { bg = { 0.08, 0.10, 0.14 }, zoom = 0,  desc = "Fast white steam that slows down (deceleration) and grows" },
    ["07_Snow"]            = { bg = { 0.04, 0.06, 0.12 }, zoom = -4, desc = "A wide spawn area and slow fall: snow from a high emitter" },
    ["08_Rain"]            = { bg = { 0.03, 0.05, 0.09 }, zoom = -3, desc = "Stretched streak sprites falling fast" },
    ["09_MagicSwirl"]      = { bg = { 0.05, 0.02, 0.09 }, zoom = 2,  desc = "Emission in every direction with strong direction noise" },
    ["10_Fireflies"]       = { bg = { 0.01, 0.03, 0.02 }, zoom = 5,  desc = "Tiny lights wandering inside a big spawn area" },
    ["11_ToxicGas"]        = { bg = { 0.03, 0.06, 0.03 }, zoom = -2, desc = "Big translucent smoke that expands slowly" },
    ["12_JetThruster"]     = { bg = { 0.02, 0.02, 0.05 }, zoom = 0,  desc = "Short, very fast particles from an emitter pointing down" },
    ["13_LavaGeyser_GPU"]  = { bg = { 0.06, 0.02, 0.01 }, zoom = -2, desc = "The same kind of emitter, drawn in GPU mode" },
    ["14_DustKickup"]      = { bg = { 0.16, 0.14, 0.12 }, zoom = -1, desc = "Brown dust that slows down quickly" },
    ["15_HealingAura"]     = { bg = { 0.02, 0.06, 0.05 }, zoom = 6,  desc = "Stars that appear small and grow while they float up" },
    ["16_RTS_SmokeBomb"]   = { bg = { 0.08, 0.09, 0.11 }, zoom = -8, desc = "The smoke bomb of the RTS: a huge, slow cloud" },
    ["17_Helix"]           = { bg = { 0.02, 0.03, 0.08 }, zoom = 4,  desc = "SWIRL + PULL: a narrow jet that winds into a helix" },
    ["18_Tornado"]         = { bg = { 0.10, 0.10, 0.12 }, zoom = -4, desc = "SWIRL + radial speed: a funnel that opens with height" },
    ["19_SpiralGalaxy"]    = { bg = { 0.01, 0.01, 0.04 }, zoom = 1,  desc = "Three arms (SPIRAL_ARMS) whose launch direction turns (SPIRAL_SPIN)" },
    ["20_Sprinkler_GPU"]   = { bg = { 0.02, 0.05, 0.10 }, zoom = 1,  desc = "One arm that sweeps around, with gravity: a garden sprinkler" },
    ["21_SphereBurst"]     = { bg = { 0.03, 0.02, 0.05 }, zoom = 4,  desc = "Shape SPHERE, surface only, direction OUTWARD" },
    ["22_TorusRing"]       = { bg = { 0.02, 0.05, 0.07 }, zoom = 4,  desc = "Shape TORUS (a ring) that also swirls around its axis" },
    ["23_ConeSpray"]       = { bg = { 0.04, 0.03, 0.03 }, zoom = 4,  desc = "Shape CONE, born on its surface and thrown outward" },
    ["24_MeshAura"]        = { bg = { 0.02, 0.05, 0.03 }, zoom = 3,  desc = "Shape MESH: the particles are born on the surface of the chess pawn" },
    ["25_Portal"]          = { bg = { 0.05, 0.02, 0.09 }, zoom = 2,  desc = "A torus turned upright and swirling: a magic portal" },
    ["26_Lightning"]       = { bg = { 0.02, 0.02, 0.05 }, zoom = 6,  desc = "Box surface, short life, lots of direction noise, in bursts", pulse = { on = 0.35, period = 0.8 } },
    ["27_Bubbles"]         = { bg = { 0.01, 0.06, 0.12 }, zoom = 3,  desc = "Born anywhere inside a sphere, rising with positive gravity" },
    ["28_Fireworks"]       = { bg = { 0.01, 0.01, 0.05 }, zoom = -3, desc = "A spherical burst with gravity, fired in bursts", pulse = { on = 0.10, period = 1.8 } },
    ["29_BlackHole"]       = { bg = { 0.00, 0.00, 0.02 }, zoom = 0,  desc = "A tilted ring whose particles swirl and fall to the centre (SWIRL_PULL)" },
    ["30_Confetti"]        = { bg = { 0.08, 0.08, 0.10 }, zoom = -2, desc = "Falls from a wide box; every piece spins (ROTATION_*)" },
}

local C_TITLE = Color.new(0.20, 0.85, 1.00, 1.0)
local C_WHITE = Color.new(1.00, 1.00, 1.00, 1.0)
local C_DIM   = Color.new(0.55, 0.55, 0.60, 1.0)
local C_ON    = Color.new(0.20, 1.00, 0.30, 1.0)
local C_OFF   = Color.new(1.00, 0.55, 0.20, 1.0)
local C_KEY   = Color.new(1.00, 0.90, 0.20, 1.0)
local C_DESC  = Color.new(0.80, 0.85, 0.95, 1.0)

local state = {
    items     = {},      -- [i] = { name, emitter, backdrop, baseY, backY, pulseOn }
    playing   = true,
    emitting  = true,
    showHUD   = true,
    autoView  = true,    -- fondo y zoom por efecto
    cur       = 0.0,     -- índice continuo del efecto que está al frente (0-based)
    from      = 0.0,
    target    = 0,       -- índice entero (sin envolver) al que se va
    glideT    = 1.0,     -- 0..1 del desplazamiento actual
    dwell     = 0.0,     -- tiempo parado en el efecto actual
    retry     = 0.0,
    ready     = false,
    t         = 0.0,     -- reloj para las ráfagas
    zoom      = 0.0,     -- desplazamiento actual de la cámara a lo largo de su eje de vista (u)
    userZoom  = 0.0,     -- el que añade la rueda del ratón
    camBase   = nil,     -- posición original de la cámara de la escena
    bg        = { 0.0, 0.0, 0.0 },   -- color de fondo actual
}

local ZOOM_STEP, ZOOM_MIN, ZOOM_MAX, ZOOM_SMOOTH, BG_SMOOTH = 3.0, -40.0, 28.0, 10.0, 3.0

local input, win, render, tw

local function cfgNum(v, def)
    v = tonumber(v)
    if v == nil then return def end
    return v
end

local function smoothstep(t)
    if t <= 0 then return 0 end
    if t >= 1 then return 1 end
    return t * t * (3 - 2 * t)
end

local function wrapIndex(i, n)
    return ((i % n) + n) % n
end

local function alive(o)
    if o == nil then return false end
    local ok, removed = pcall(function() return o:isRemoved() end)
    return ok and not removed
end

-- The automatic view only ever steps BACK (zoom <= 0): moving the camera towards a small effect put it inside the particles and the
-- effect disappeared. The wheel still zooms in on demand.
local function moodOf(name)
    local m = MOODS[name] or { bg = DEFAULT_BG, zoom = 0, desc = "" }
    if m.zoom > 0 then m = { bg = m.bg, zoom = 0, desc = m.desc, pulse = m.pulse } end
    return m
end

local function currentName()
    return EFFECTS[wrapIndex(state.target, #EFFECTS) + 1]
end

-- ¿Debe estar emitiendo este emisor ahora? (emisión general + su ráfaga, si la tiene)
local function wantsEmission(it)
    if not state.emitting then return false end
    local pulse = moodOf(it.name).pulse
    if pulse == nil then return true end
    return (state.t % pulse.period) < pulse.on
end

-- Las escenas cargan de forma asíncrona: se van resolviendo los objetos que faltan
local function resolveItems()
    local missing = 0
    for i, name in ipairs(EFFECTS) do
        local it = state.items[i]
        if it == nil then
            it = { name = name }
            state.items[i] = it
        end
        if not alive(it.emitter) then
            local e = Brakeza:getParticleEmitterByName(name)
            if e ~= nil then
                it.emitter = e
                it.baseY = e:getPosition().y
                it.pulseOn = wantsEmission(it)
                e:setStopAdd(not it.pulseOn)
            end
        end
        if not alive(it.backdrop) then
            local b = Brakeza:getObjectByName("Backdrop_" .. name)
            if b ~= nil then
                it.backdrop = b
                it.backY = b:getPosition().y
            end
        end
        if it.emitter == nil then missing = missing + 1 end
    end
    state.ready = (missing == 0)
end

-- Las ráfagas: solo se llama a setStopAdd cuando el estado cambia
local function updatePulses()
    for _, it in ipairs(state.items) do
        if alive(it.emitter) then
            local on = wantsEmission(it)
            if on ~= it.pulseOn then
                it.pulseOn = on
                it.emitter:setStopAdd(not on)
            end
        end
    end
end

local function layout()
    local n = #EFFECTS
    local radius  = cfgNum(radius, 33.0)
    local cx      = cfgNum(centerX, 0.0)
    local cz      = cfgNum(centerZ, 9.0)
    local step    = 2 * math.pi / n

    for i, it in ipairs(state.items) do
        -- ángulo 0 = justo delante (hacia la cámara, +Z); el índice `cur` es el que queda ahí
        local a = (i - 1 - state.cur) * step
        local s, c = math.sin(a), math.cos(a)
        if alive(it.emitter) then
            it.emitter:setPosition(Vertex3D.new(cx + radius * s, it.baseY or 0, cz + radius * c))
        end
        if alive(it.backdrop) then
            -- la peana está justo bajo el emisor (y el peón de 24_MeshAura es la propia fuente de partículas)
            it.backdrop:setPosition(Vertex3D.new(cx + radius * s, it.backY or 0, cz + radius * c))
        end
    end
end

local function goTo(newTarget)
    state.from   = state.cur
    state.target = newTarget
    state.glideT = 0.0
    state.dwell  = 0.0
end

local function setEmitting(on)
    state.emitting = on
    updatePulses()
end

-- Zoom automático del efecto + el de la rueda del ratón, suavizado: dolly de la cámara por su eje de vista
local function updateZoom(dt)
    local camera = Components:Camera():getCamera()
    if state.camBase == nil then
        local p = camera:getPosition()
        state.camBase = { x = p.x, y = p.y, z = p.z }
    end

    local wheel = input:getMouseWheelY()
    if wheel ~= 0 then
        state.userZoom = math.max(ZOOM_MIN, math.min(ZOOM_MAX, state.userZoom + wheel * ZOOM_STEP))
    end

    local goal = state.userZoom
    if state.autoView then goal = goal + moodOf(currentName()).zoom end
    goal = math.max(ZOOM_MIN, math.min(ZOOM_MAX, goal))
    state.zoom = state.zoom + (goal - state.zoom) * math.min(1.0, ZOOM_SMOOTH * dt)

    local f = camera:AxisForward()
    local b = state.camBase
    camera:setPosition(Vertex3D.new(b.x + f.x * state.zoom, b.y + f.y * state.zoom, b.z + f.z * state.zoom))
end

-- La cámara mira al efecto que está al frente (a media altura de su columna de partículas), con un giro suave: así el efecto queda
-- centrado en pantalla en vez de en el borde inferior
local LOOK_HEIGHT, LOOK_SMOOTH = 3.0, 4.0
local function updateLookAt(dt)
    local b = state.camBase
    if b == nil then return end
    local it = state.items[wrapIndex(state.target, #EFFECTS) + 1]
    if it == nil or not alive(it.emitter) then return end

    local ringRadius = cfgNum(radius, 33.0)
    local tx, ty, tz = cfgNum(centerX, 0.0), (it.baseY or 0) + LOOK_HEIGHT, cfgNum(centerZ, 9.0) + ringRadius
    local dx, dy, dz = tx - b.x, ty - b.y, tz - b.z
    local yaw     = math.deg(math.atan2(dx, -dz))
    local pitchUp = math.deg(math.atan2(dy, math.sqrt(dx * dx + dz * dz)))

    if state.lookYaw == nil then state.lookYaw, state.lookPitch = yaw, pitchUp end
    local k = math.min(1.0, LOOK_SMOOTH * dt)
    state.lookYaw   = state.lookYaw   + (yaw     - state.lookYaw)   * k
    state.lookPitch = state.lookPitch + (pitchUp - state.lookPitch) * k
    Components:Camera():getCamera():setRotationFromEulerAngles(-state.lookPitch, state.lookYaw, 0)
end

-- Fondo del efecto que está al frente, con transición suave
local function updateBackground(dt)
    local goal = state.autoView and moodOf(currentName()).bg or { 0.0, 0.0, 0.0 }
    local k = math.min(1.0, BG_SMOOTH * dt)
    for i = 1, 3 do
        state.bg[i] = state.bg[i] + (goal[i] - state.bg[i]) * k
    end
    win:setClearColor(state.bg[1], state.bg[2], state.bg[3], 0.0)
end

local function drawHUD()
    if not state.showHUD then
        tw:writeTextAtlasCache(14, 14, "H - show panel", C_DIM, 0.52)
        return
    end
    local n   = #EFFECTS
    local name = currentName()
    local idx = wrapIndex(state.target, n) + 1
    local label = name:gsub("^%d+_", ""):gsub("_", " ")

    tw:writeTextAtlasCache(14, 40, "PARTICLE EFFECTS", C_TITLE, 0.70)
    tw:writeTextAtlas(14, 86, label .. "  (" .. idx .. "/" .. n .. ")", C_WHITE, 0.80)
    tw:writeTextAtlas(14, 118, moodOf(name).desc, C_DESC, 0.50)
    tw:writeTextAtlas(14, 148, state.playing and "PLAYING" or "PAUSED", state.playing and C_ON or C_OFF, 0.62)
    tw:writeTextAtlas(14, 174, "Emission: " .. (state.emitting and "ON" or "OFF") ..
        "     View: " .. (state.autoView and "AUTO" or "manual"), state.emitting and C_ON or C_OFF, 0.55)
    tw:writeTextAtlas(14, 200, string.format("Time per effect: %.0f s", cfgNum(dwellTime, 6.0)), C_DIM, 0.55)

    local y = 238
    local keys = {
        { "Space",       "play / pause" },
        { "Left/Right",  "previous / next" },
        { "Up/Down",     "time per effect" },
        { "E",           "emission on/off" },
        { "M",           "auto view (background + zoom)" },
        { "Wheel",       "zoom" },
        { "H",           "hide panel" },
    }
    for _, k in ipairs(keys) do
        tw:writeTextAtlasCache(14, y, k[1], C_KEY, 0.55)
        tw:writeTextAtlasCache(150, y, k[2], C_DIM, 0.55)
        y = y + 24
    end
end

function onStart()
    input  = Components:Input()
    win    = Components:Window()
    render = Components:Render()
    tw     = render:getTextWriter()
    state.items, state.ready, state.started = {}, false, false
    state.playing, state.emitting, state.showHUD, state.autoView = true, true, true, true
    state.cur, state.from, state.target, state.glideT, state.dwell = 0.0, 0.0, 0, 1.0, 0.0
    state.retry, state.t = 0.0, 0.0
    state.zoom, state.userZoom, state.camBase = 0.0, 0.0, nil
    state.bg = { 0.0, 0.0, 0.0 }
    print("[ParticleCarousel] Space = play/pause, Left/Right = prev/next, E = emission, M = auto view, H = panel")
end

local function _update()
    input  = Components:Input()
    win    = Components:Window()
    render = Components:Render()
    tw     = render:getTextWriter()

    local dt = Brakeza:getDeltaTime()
    state.t = state.t + dt

    -- Resolver objetos (cada 0.25 s mientras falte alguno)
    if not state.ready then
        state.retry = state.retry - dt
        if state.retry <= 0 then
            state.retry = 0.25
            resolveItems()
            if state.ready and not state.started then
                -- arranca en el último efecto (confeti) en vez del primero: más vistoso para abrir la demo
                state.started = true
                state.cur, state.from, state.target = #EFFECTS - 1, #EFFECTS - 1, #EFFECTS - 1
            end
        end
    end

    -- Entrada
    if input:isCharFirstEventDown("Space") then
        state.playing = not state.playing
        state.dwell = 0.0
    end
    if input:isCharFirstEventDown("Right") then goTo(math.floor(state.target + 0.5) + 1) end
    if input:isCharFirstEventDown("Left")  then goTo(math.floor(state.target + 0.5) - 1) end
    if input:isCharFirstEventDown("E")     then setEmitting(not state.emitting) end
    if input:isCharFirstEventDown("M")     then state.autoView = not state.autoView end
    if input:isCharFirstEventDown("H")     then state.showHUD = not state.showHUD end
    if input:isCharFirstEventDown("Up")    then dwellTime = math.min(60, cfgNum(dwellTime, 6.0) + 1) end
    if input:isCharFirstEventDown("Down")  then dwellTime = math.max(1,  cfgNum(dwellTime, 6.0) - 1) end

    -- Desplazamiento suave hacia el efecto elegido
    local glide = math.max(0.1, cfgNum(glideTime, 1.2))
    if state.glideT < 1.0 then
        state.glideT = math.min(1.0, state.glideT + dt / glide)
        state.cur = state.from + (state.target - state.from) * smoothstep(state.glideT)
    else
        state.cur = state.target
        -- Parado en un efecto: en play, pasa al siguiente tras `dwellTime`
        if state.playing then
            state.dwell = state.dwell + dt
            if state.dwell >= cfgNum(dwellTime, 6.0) then goTo(state.target + 1) end
        end
    end

    updatePulses()
    updateLookAt(dt)
    updateZoom(dt)
    updateBackground(dt)
    layout()
    drawHUD()
end

function onUpdate()
    local ok, err = pcall(_update)
    if not ok then
        print("[ParticleCarousel] ERROR: " .. tostring(err))
    end
end

function onEnd()
    -- devolver la cámara y el fondo a su sitio original
    pcall(function() Components:Window():setClearColor(0.0, 0.0, 0.0, 0.0) end)
    if state.camBase ~= nil then
        pcall(function()
            local b = state.camBase
            Components:Camera():getCamera():setPosition(Vertex3D.new(b.x, b.y, b.z))
        end)
    end
    state.items = {}
end
