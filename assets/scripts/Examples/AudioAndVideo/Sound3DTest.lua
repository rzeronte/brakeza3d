-- Sound3DTest.lua
-- Demo de sonido posicional (Sound3D): fuentes fijas y una que orbita alrededor del centro. El volumen sube al acercar
-- la cámara (la "oyente") y, con el paneo activado, el sonido se desplaza al canal izquierdo o derecho según
-- dónde queda la fuente respecto a ti. El panel muestra distancia, volumen y paneo de cada fuente.
--
-- CONTROLS
--   Arrows               mover la cámara (la oyente)       Shift + Up/Down   subir / bajar
--   Mouse wheel          avanzar / retroceder              Middle button + drag   mirar
--   1 - 4                silenciar / activar cada fuente
--   M                    silenciar / activar todo el ambiente
--   P                    paneo izquierda/derecha on / off
--   R                    mostrar / ocultar los anillos (verde = volumen máximo, naranja = silencio)
--   H                    mostrar / ocultar el panel
--
-- Cómo se usa Sound3D desde Lua:
--   local s = ObjectFactory.Sound3D(fichero, posicion, radioInterior, radioExterior, volumen)
--   s:setSoundLoop(true)
-- Dentro del radio interior suena al volumen base; entre el interior y el exterior baja en línea recta
-- hasta cero; fuera del exterior no suena (la voz se libera).

local tw, render, input, sound

local C_TITLE  = Color.new(1.00, 1.00, 0.30, 1.0)
local C_ACTIVE = Color.new(0.20, 1.00, 0.20, 1.0)
local C_MID    = Color.new(1.00, 0.75, 0.15, 1.0)
local C_SILENT = Color.new(0.45, 0.45, 0.45, 1.0)
local C_MUTED  = Color.new(0.90, 0.30, 0.30, 1.0)
local C_HINT   = Color.new(0.60, 0.60, 0.65, 1.0)
local C_KEY    = Color.new(1.00, 0.90, 0.20, 1.0)

local ORBIT_CENTER = { x = 0, y = 0.5, z = -10 }
local ORBIT_RADIUS = 7.0
local ORBIT_SPEED  = 0.55      -- rad/s

local SOURCES = {
    { name = "sound_beep_left",      file = "../assets/sounds/radio_beep.wav",
      pos = { -12, 0, -10 }, inner = 5, outer = 22, volume = 110, label = "Radio beep (left)" },
    { name = "sound_ambient_center", file = "../assets/sounds/tutorial/music_loop.wav",
      pos = { 0, 0, -10 },   inner = 8, outer = 35, volume = 90,  label = "Music loop (centre)" },
    { name = "sound_beep_right",     file = "../assets/sounds/radio_beep.wav",
      pos = { 12, 0, -10 },  inner = 3, outer = 15, volume = 128, label = "Close beep (right)" },
    { name = "sound_beep_orbit",     file = "../assets/sounds/radio_beep.wav",
      pos = { ORBIT_CENTER.x + ORBIT_RADIUS, ORBIT_CENTER.y, ORBIT_CENTER.z }, inner = 2, outer = 14, volume = 128,
      label = "Orbiting beep", orbit = true },
}

local S = {
    t = 0.0, muted = false, panning = true, rings = true, showHUD = true,
    created = {},      -- [i] = { snd = Sound3D, marker = Mesh3D|nil, on = bool }
}

local function alive(o)
    if o == nil then return false end
    local ok, removed = pcall(function() return o:isRemoved() end)
    return ok and not removed
end

local function dist3(a, b)
    local dx, dy, dz = a.x - b.x, a.y - b.y, a.z - b.z
    return math.sqrt(dx * dx + dy * dy + dz * dz)
end

-- Mismo cálculo que Sound3D::onUpdate (motor): lineal entre el radio interior y el exterior
local function volumeOf(src, d)
    if d <= src.inner then return src.volume end
    if d >= src.outer then return 0 end
    return src.volume * (1 - (d - src.inner) / (src.outer - src.inner))
end

-- Mismo paneo que el motor: componente de la dirección a la fuente sobre el eje derecho de la cámara
local function panOf(cam, camPos, p)
    local dx, dy, dz = p.x - camPos.x, p.y - camPos.y, p.z - camPos.z
    local len = math.sqrt(dx * dx + dy * dy + dz * dz)
    if len < 0.001 then return 0 end
    local r = cam:getRotation():getTranspose() * Vertex3D.new(1, 0, 0)
    local side = (dx * r.x + dy * r.y + dz * r.z) / len
    return math.max(-1, math.min(1, side)) * math.max(0, math.min(1, len / 2))
end

local function bar(frac, width)
    local filled = math.floor(frac * width + 0.5)
    return string.rep("#", filled) .. string.rep("-", width - filled)
end

-- "L ---|--#--- R": marca dónde cae el sonido entre los dos canales
local function panBar(pan, width)
    local cells = {}
    for i = 1, width do cells[i] = "-" end
    cells[math.ceil(width / 2)] = "|"
    local idx = math.max(1, math.min(width, math.floor((pan + 1) / 2 * (width - 1) + 1.5)))
    cells[idx] = "#"
    return "L " .. table.concat(cells) .. " R"
end

function onStart()
    Components:Camera():setEnabled(true)
    Components:Input():setEnabled(true)
    Components:Input():setKeyboardEnabled(true)
    Components:Input():setMouseEnabled(true)

    tw     = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
    render = Components:Render()
    input  = Components:Input()
    sound  = Components:Sound()

    S.t, S.muted, S.panning, S.rings, S.showHUD = 0.0, false, true, true, true
    S.created = {}

    pcall(function() sound:setAmbienceVolume(100) end)
    pcall(function() sound:setAmbiencePanning(true) end)   -- apagado por defecto en el motor; esta demo lo enciende (exe antiguo: se ignora)

    for i, src in ipairs(SOURCES) do
        local pos = Vertex3D.new(src.pos[1], src.pos[2], src.pos[3])
        local snd = ObjectFactory.Sound3D(src.file, pos, src.inner, src.outer, src.volume)
        snd:setName(src.name)
        snd:setSoundLoop(true)

        -- La fuente que orbita no tiene malla en la escena: se crea una para ver por dónde va
        local marker = nil
        if src.orbit then
            marker = ObjectFactory.Mesh3D("../assets/models/basic/icosphere.fbx", pos)
            marker:setName("Marker_Orbit")
            marker:setScale(0.45)
        end
        S.created[i] = { snd = snd, marker = marker, on = true }
    end
    print("[Sound3DTest] " .. #SOURCES .. " Sound3D sources created")
end

local function setSourceOn(i, on)
    local c = S.created[i]
    if c == nil or not alive(c.snd) then return end
    c.on = on
    c.snd:setEnabled(on and not S.muted)
end

local function applyMuteAll()
    for i, c in ipairs(S.created) do
        if alive(c.snd) then c.snd:setEnabled(c.on and not S.muted) end
    end
end

local function handleInput()
    for i = 1, #SOURCES do
        if input:isCharFirstEventDown(tostring(i)) and S.created[i] ~= nil then
            setSourceOn(i, not S.created[i].on)
        end
    end
    if input:isCharFirstEventDown("M") then
        S.muted = not S.muted
        applyMuteAll()
    end
    if input:isCharFirstEventDown("P") then
        S.panning = not S.panning
        pcall(function() sound:setAmbiencePanning(S.panning) end)
    end
    if input:isCharFirstEventDown("R") then S.rings = not S.rings end
    if input:isCharFirstEventDown("H") then S.showHUD = not S.showHUD end
end

function onUpdate()
    local ok, err = pcall(function()
        input = Components:Input()
        handleInput()

        -- La fuente que orbita: se mueve el Sound3D y su marcador a la vez
        S.t = S.t + Brakeza:getDeltaTime()
        for i, src in ipairs(SOURCES) do
            local c = S.created[i]
            if src.orbit and c ~= nil and alive(c.snd) then
                local a = S.t * ORBIT_SPEED
                local p = Vertex3D.new(ORBIT_CENTER.x + math.cos(a) * ORBIT_RADIUS, ORBIT_CENTER.y,
                                       ORBIT_CENTER.z + math.sin(a) * ORBIT_RADIUS)
                c.snd:setPosition(p)
                if alive(c.marker) then c.marker:setPosition(p) end
            end
        end
    end)
    if not ok then print("[Sound3DTest] ERROR: " .. tostring(err)) end
end

function postUpdate()
    if tw == nil then return end
    local ok, err = pcall(function()
        local cam    = Components:Camera():getCamera()
        local camPos = cam:getPosition()

        -- Anillos en el suelo de cada fuente: verde = radio interior, naranja = radio exterior
        if S.rings then
            for i, src in ipairs(SOURCES) do
                local c = S.created[i]
                if c ~= nil and alive(c.snd) then
                    local alpha = (c.on and not S.muted) and 0.9 or 0.25
                    render:drawGroundCircle(c.snd, 0.2, 1.0, 0.2, alpha, src.inner, 0.15)
                    render:drawGroundCircle(c.snd, 1.0, 0.55, 0.1, alpha * 0.6, src.outer,
                        math.max(0.1, (src.outer - src.inner) * 0.005))
                end
            end
        end

        if not S.showHUD then
            tw:writeTextTTFAutoSize(20, 20, "H - show panel", C_HINT, 0.5)
            return
        end

        local X, Y, LH = 20, 20, 24
        tw:writeTextTTFAutoSize(X, Y, "Sound3D", C_TITLE, 0.80);                       Y = Y + LH + 8
        tw:writeTextTTFAutoSize(X, Y, "Walk the camera towards each source to hear it", C_HINT, 0.5); Y = Y + LH + 6
        tw:writeTextTTFAutoSize(X, Y, string.format("Panning %s    Ambience %s",
            S.panning and "ON" or "OFF", S.muted and "MUTED" or "on"),
            S.muted and C_MUTED or C_ACTIVE, 0.55)
        Y = Y + LH + 6

        for i, src in ipairs(SOURCES) do
            local c = S.created[i]
            local pos = (c ~= nil and alive(c.snd)) and c.snd:getPosition() or Vertex3D.new(src.pos[1], src.pos[2], src.pos[3])
            local d = dist3(camPos, pos)
            local vol = volumeOf(src, d)
            local frac = vol / 128
            local active = c ~= nil and c.on and not S.muted

            local state, col
            if not active then state, col = "MUTED", C_MUTED
            elseif vol >= src.volume then state, col = "MAX", C_ACTIVE
            elseif vol > 0 then state, col = string.format("%3d%%", math.floor(vol / src.volume * 100)), C_MID
            else state, col = "SILENT", C_SILENT end

            local pan = S.panning and panOf(cam, camPos, pos) or 0
            tw:writeTextTTFAutoSize(X, Y, string.format("[%d] %-20s d=%5.1f [%s] %-6s %s",
                i, src.label, d, bar(active and frac or 0, 10), state,
                (active and vol > 0) and panBar(pan, 11) or ""), col, 0.5)
            Y = Y + LH
        end

        Y = Y + 8
        local keys = { "1-4 mute a source", "M mute all", "P panning", "R rings", "H hide panel" }
        tw:writeTextTTFAutoSize(X, Y, table.concat(keys, "   "), C_KEY, 0.45)
        Y = Y + LH
        tw:writeTextTTFAutoSize(X, Y, "Move: arrows / wheel / Shift+Up,Down.  Look: hold middle mouse button and drag", C_HINT, 0.45)
    end)
    if not ok then print("[Sound3DTest] HUD ERROR: " .. tostring(err)) end
end

function onEnd()
    -- Devolver el ambiente a su estado normal y retirar lo creado por el script
    pcall(function() Components:Sound():setAmbiencePanning(false) end)
    for _, c in ipairs(S.created) do
        if alive(c.snd) then c.snd:setRemoved(true) end
        if alive(c.marker) then c.marker:setRemoved(true) end
    end
    S.created = {}
    print("[Sound3DTest] stopped")
end
