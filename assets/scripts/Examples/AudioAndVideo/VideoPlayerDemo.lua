-- VideoPlayerDemo.lua
-- Demo de reproducción de vídeo en dos formatos: a PANTALLA COMPLETA y en un PLANO (una ventana de vídeo con marco que
-- ocupa solo una parte de la pantalla, con la escena visible alrededor). Se puede pasar de uno a otro mientras suena.
-- Cuando el vídeo termina, el script reacciona (aquí, el toroide da un salto). Es el patrón del vídeo de introducción del RTS.
--
-- CONTROLS
--   V                  reproducir a pantalla completa (si ya suena, lo reinicia)
--   B                  reproducir en el plano / ventana (si ya suena, lo reinicia)
--   F                  cambiar entre pantalla completa y plano mientras suena
--   any other key / click   saltar el vídeo mientras se reproduce
--   H                  mostrar / ocultar el panel
--
-- Cómo se usa desde Lua:
--   local video = ObjectFactory.Image2D("", 0, 0, 1.0, 1.0)   -- Image2D vacío
--   video:loadVideo("../assets/videos/video.mp4")             -- empieza a reproducir
--   video:setSize(w, h)                                       -- tamaño como FRACCIÓN de la pantalla (1.0 = pantalla completa)
--   video:setScreenPosition(x, y)                             -- esquina superior izquierda, en píxeles
--   video:isVideoFinished()                                   -- true cuando ha llegado al final
--   video:stopVideo(); video:setRemoved(true)                 -- parar y quitar
-- (ObjectFactory.PlayVideoCutscene(ruta, fundido) reproduce un vídeo de forma bloqueante, sin script.)
--
-- Variables del script (VideoPlayerDemo.json): targetName = objeto que salta al terminar el vídeo;
-- jumpOffset = cuánto salta.

local VIDEO_PATH      = "../assets/videos/video.mp4"
local VIDEO_W, VIDEO_H = 640, 360      -- resolución del vídeo (16:9): el plano conserva esa proporción
local VIDEO_SECONDS   = 30.0           -- duración aproximada, solo para la barra de progreso del panel
local WINDOW_WIDTH    = 0.50           -- ancho del plano como fracción de la pantalla
local FRAME_PX        = 8              -- grosor del marco del plano
local SKIP_GRACE_TIME = 0.4            -- s: se ignora la entrada tras empezar, o la propia pulsación de V/B lo saltaría

local C_TITLE = Color.new(0.20, 0.85, 1.00, 1.0)
local C_WHITE = Color.new(1.00, 1.00, 1.00, 1.0)
local C_DIM   = Color.new(0.55, 0.55, 0.60, 1.0)
local C_ON    = Color.new(0.20, 1.00, 0.30, 1.0)
local C_KEY   = Color.new(1.00, 0.90, 0.20, 1.0)
local C_FRAME = Color.new(0.10, 0.12, 0.18, 1.0)
local C_BAR   = Color.new(0.20, 0.85, 1.00, 1.0)
local C_BARBG = Color.new(0.20, 0.22, 0.28, 1.0)

local S = {
    video = nil, playing = false, fullscreen = true, elapsed = 0.0, plays = 0, finishedCount = 0,
    target = nil, targetHome = nil, atHome = true, t = 0.0, showHUD = true,
    rect = { x = 0, y = 0, w = 0, h = 0 },     -- dónde está el vídeo en pantalla (píxeles), para el marco y la barra
}
local input, win, render, tw

local function alive(o)
    if o == nil then return false end
    local ok, removed = pcall(function() return o:isRemoved() end)
    return ok and not removed
end

local function stopVideo()
    if S.video ~= nil then
        pcall(function() S.video:stopVideo() end)
        if alive(S.video) then S.video:setRemoved(true) end
    end
    S.video, S.playing = nil, false
end

-- Coloca el vídeo según el modo. Image2D mide su tamaño como fracción de la pantalla, así que el plano
-- calcula el alto para conservar la proporción del vídeo con la proporción real de la ventana.
local function applyLayout()
    if not alive(S.video) then return end
    local sw, sh = win:getWidth(), win:getHeight()
    if sw <= 0 or sh <= 0 then return end

    if S.fullscreen then
        S.video:setSize(1.0, 1.0)
        S.video:setScreenPosition(0, 0)
        S.rect.x, S.rect.y, S.rect.w, S.rect.h = 0, 0, sw, sh
    else
        local fw = WINDOW_WIDTH
        local fh = fw * (sw / sh) * (VIDEO_H / VIDEO_W)
        local pw, ph = math.floor(fw * sw), math.floor(fh * sh)
        local px = math.floor((sw - pw) * 0.5)
        local py = math.floor((sh - ph) * 0.5) + math.floor(sh * 0.06)     -- un poco abajo: deja sitio al panel
        S.video:setSize(fw, fh)
        S.video:setScreenPosition(px, py)
        S.rect.x, S.rect.y, S.rect.w, S.rect.h = px, py, pw, ph
    end
end

local function playVideo(fullscreen)
    stopVideo()
    local f = io.open(VIDEO_PATH, "rb")
    if f == nil then
        print("[VideoPlayerDemo] video not found: " .. VIDEO_PATH)
        return
    end
    f:close()

    S.fullscreen = fullscreen
    S.video = ObjectFactory.Image2D("", 0, 0, 1.0, 1.0)
    S.video:loadVideo(VIDEO_PATH)
    S.playing, S.elapsed = true, 0.0
    S.plays = S.plays + 1
    applyLayout()
end

-- Lo que pasa cuando el vídeo termina: el objeto da un salto de ida o de vuelta
local function onVideoFinished()
    S.finishedCount = S.finishedCount + 1
    if not alive(S.target) or S.targetHome == nil then return end
    local off = jumpOffset
    if off == nil then off = Vertex3D.new(6, 0, 0) end
    local home = S.targetHome
    S.atHome = not S.atHome
    if S.atHome then
        S.target:setPosition(Vertex3D.new(home.x, home.y, home.z))
    else
        S.target:setPosition(Vertex3D.new(home.x + off.x, home.y + off.y, home.z + off.z))
    end
end

function onStart()
    input  = Components:Input()
    win    = Components:Window()
    render = Components:Render()
    tw     = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
    S.video, S.playing, S.fullscreen, S.elapsed, S.plays, S.finishedCount = nil, false, true, 0.0, 0, 0
    S.atHome, S.t, S.showHUD = true, 0.0, true
    S.target = Brakeza:getObjectByName((targetName ~= nil and targetName ~= "") and targetName or "VideoTarget")
    if S.target ~= nil then
        local p = S.target:getPosition()
        S.targetHome = { x = p.x, y = p.y, z = p.z }
    end
end

function onUpdate()
    local ok, err = pcall(function()
        input, win, render = Components:Input(), Components:Window(), Components:Render()
        local dt = Brakeza:getDeltaTime()

        local pressedV = input:isCharFirstEventDown("V")
        local pressedB = input:isCharFirstEventDown("B")
        local pressedF = input:isCharFirstEventDown("F")
        if input:isCharFirstEventDown("H") then S.showHUD = not S.showHUD end

        if pressedV then playVideo(true) end
        if pressedB then playVideo(false) end
        if pressedF and S.playing then S.fullscreen = not S.fullscreen end

        if S.playing then
            S.elapsed = S.elapsed + dt
            applyLayout()

            -- Saltar con cualquier otra tecla o clic (V, B y F tienen su función y no saltan)
            local own = pressedV or pressedB or pressedF
            local skip = S.elapsed > SKIP_GRACE_TIME and not own and (input:isKeyEventDown() or input:isMouseButtonDown())
            if skip or S.video == nil or S.video:isVideoFinished() then
                local finished = (not skip) and S.video ~= nil
                stopVideo()
                if finished then onVideoFinished() end
            end

            -- Marco del plano: se dibuja ANTES que el vídeo (los objetos se pintan después de los scripts)
            if S.playing and not S.fullscreen then
                local r = S.rect
                render:DrawFilledRect(r.x - FRAME_PX, r.y - FRAME_PX, r.w + FRAME_PX * 2, r.h + FRAME_PX * 2, C_FRAME)
            end
        end

        -- El objeto sube y baja suavemente para ver que la escena sigue viva mientras no hay vídeo a pantalla completa
        S.t = S.t + dt
        if alive(S.target) and S.targetHome ~= nil and S.atHome then
            local home = S.targetHome
            S.target:setPosition(Vertex3D.new(home.x, home.y + math.sin(S.t * 1.5) * 0.4, home.z))
        end
    end)
    if not ok then print("[VideoPlayerDemo] ERROR: " .. tostring(err)) end
end

function postUpdate()
    if tw == nil then return end
    if S.playing and S.fullscreen then return end      -- a pantalla completa solo se ve el vídeo
    pcall(function()
        if not S.showHUD then
            tw:writeTextTTFAutoSize(20, 20, "H - show panel", C_DIM, 0.5)
            return
        end
        local X, Y, LH = 20, 20, 26
        tw:writeTextTTFAutoSize(X, Y, "Video player", C_TITLE, 0.80);                              Y = Y + LH + 8
        if S.playing then
            tw:writeTextTTFAutoSize(X, Y, "Playing in a plane (F: switch to full screen)", C_ON, 0.6)
        else
            tw:writeTextTTFAutoSize(X, Y, "V: full screen     B: plane (window with a frame)", C_WHITE, 0.6)
        end
        Y = Y + LH + 4
        tw:writeTextTTFAutoSize(X, Y, string.format("Played %d time(s), finished %d time(s)", S.plays, S.finishedCount),
            S.finishedCount > 0 and C_ON or C_DIM, 0.5);                                           Y = Y + LH
        tw:writeTextTTFAutoSize(X, Y, "When it finishes the torus jumps; skipping it does not count", C_DIM, 0.45); Y = Y + LH + 8
        tw:writeTextTTFAutoSize(X, Y, "V full screen   B plane   F switch   any other key or click skips   H hide panel", C_KEY, 0.45)

        -- Barra de progreso bajo el plano
        if S.playing then
            local r = S.rect
            local frac = math.max(0.0, math.min(1.0, S.elapsed / VIDEO_SECONDS))
            local by = r.y + r.h + FRAME_PX + 10
            render:DrawFilledRect(r.x, by, r.w, 8, C_BARBG)
            render:DrawFilledRect(r.x, by, math.floor(r.w * frac), 8, C_BAR)
            tw:writeTextTTFAutoSize(r.x, by + 14, string.format("%.0f s / ~%.0f s", S.elapsed, VIDEO_SECONDS), C_DIM, 0.45)
        end
    end)
end

function onEnd()
    stopVideo()
    S.target = nil
end
