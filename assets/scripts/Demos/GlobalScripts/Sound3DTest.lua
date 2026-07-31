-- Sound3DTest.lua
-- Crea tres fuentes Sound3D en posiciones fijas y muestra un panel HUD
-- con la distancia de la cámara a cada fuente y su volumen calculado.

local tw     = nil
local render = nil
local camera = nil
local input  = nil

local C_TITLE  = Color.new(1.00, 1.00, 0.30, 1.0)
local C_ACTIVE = Color.new(0.20, 1.00, 0.20, 1.0)
local C_MID    = Color.new(1.00, 0.75, 0.15, 1.0)
local C_SILENT = Color.new(0.45, 0.45, 0.45, 1.0)
local C_HINT   = Color.new(0.60, 0.60, 0.65, 1.0)
local C_LABEL  = Color.new(0.80, 0.80, 0.80, 1.0)

local SOURCES = {
    {
        name   = "sound_beep_left",
        file   = "../assets/sounds/radio_beep.wav",
        pos    = Vertex3D.new(-12, 0, -10),
        inner  = 5,
        outer  = 22,
        volume = 110,
        loop   = true,
        label  = "Radio Beep (left)",
    },
    {
        name   = "sound_ambient_center",
        file   = "../assets/sounds/RTS/atmosphere_day.wav",
        pos    = Vertex3D.new(0, 0, -10),
        inner  = 8,
        outer  = 35,
        volume = 90,
        loop   = true,
        label  = "Day Ambient (center)",
    },
    {
        name   = "sound_beep_right",
        file   = "../assets/sounds/radio_beep.wav",
        pos    = Vertex3D.new(12, 0, -10),
        inner  = 3,
        outer  = 15,
        volume = 128,
        loop   = true,
        label  = "Close Beep (right)",
    },
}

local function dist3(a, b)
    local dx, dy, dz = a.x - b.x, a.y - b.y, a.z - b.z
    return math.sqrt(dx*dx + dy*dy + dz*dz)
end

local function volPercent(src, dist)
    if dist <= src.inner then return 100 end
    if dist >= src.outer then return 0   end
    local t = (dist - src.inner) / (src.outer - src.inner)
    return math.floor((1 - t) * 100)
end

function onStart()
    Components:Camera():setEnabled(true)
    Components:Input():setEnabled(true)
    Components:Input():setKeyboardEnabled(true)
    Components:Input():setMouseEnabled(true)

    tw     = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
    render = Components:Render()
    input  = Components:Input()
    camera = Components:Camera():getCamera()

    for _, src in ipairs(SOURCES) do
        local snd = ObjectFactory.Sound3D(src.file, src.pos, src.inner, src.outer, src.volume)
        snd:setName(src.name)
        snd:setSoundLoop(src.loop)
        print("[Sound3DTest] created " .. src.name)
    end

    print("[Sound3DTest] onStart OK — 3 Sound3D objects spawned")
end

function postUpdate()
    local cam    = Components:Camera():getCamera()
    local camPos = cam:getPosition()

    local PAD = 20
    local X   = PAD
    local Y   = PAD
    local LH  = 26

    tw:writeTextTTFAutoSize(X, Y, "Sound3D Test", C_TITLE, 0.80)
    Y = Y + LH + 6

    tw:writeTextTTFAutoSize(X, Y, "Move the camera toward each marker to hear it", C_HINT, 0.55)
    Y = Y + LH + 10

    for _, src in ipairs(SOURCES) do
        local d   = dist3(camPos, src.pos)
        local pct = volPercent(src, d)

        local stateStr, col
        if pct == 100 then
            stateStr = "MAX VOL"
            col = C_ACTIVE
        elseif pct > 0 then
            stateStr = "VOL " .. pct .. "%"
            col = C_MID
        else
            stateStr = "SILENT"
            col = C_SILENT
        end

        local bar = ""
        local filled = math.floor(pct / 10)
        for i = 1, 10 do
            bar = bar .. (i <= filled and "#" or "-")
        end

        tw:writeTextTTFAutoSize(X, Y,
            string.format("%-22s  dist=%5.1f  [%s]  %s",
                src.label, d, bar, stateStr),
            col, 0.55)
        Y = Y + LH
    end

    Y = Y + 10
    tw:writeTextTTFAutoSize(X, Y,
        "inner=full volume  outer=silence boundary",
        C_HINT, 0.48)
    Y = Y + LH
    tw:writeTextTTFAutoSize(X, Y,
        "Sound3D objects are invisible — select them in scene viewer to inspect",
        C_HINT, 0.48)
end

function onEnd()
    print("[Sound3DTest] stopped")
end
