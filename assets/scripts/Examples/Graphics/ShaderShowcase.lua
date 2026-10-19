-- ShaderShowcase.lua
-- Escaparate de los shaders de nodos de assets/shaders/Examples (NodeMesh3D y NodePostprocessing).
--
--   * Las figuras de las dos filas llevan cada una un shader de OBJETO distinto (se ve en la propia figura).
--   * Los shaders de PANTALLA (post-proceso) están todos cargados en la escena pero apagados; este script
--     enciende de uno en uno el que toque.
--
-- CONTROLS
--   Left / Right   efecto de pantalla anterior / siguiente
--   0              sin efecto de pantalla
--   Space          cambio automático de efecto (cada 4 s) activado / desactivado
--   R              girar las figuras / dejarlas quietas
--   H              mostrar / ocultar panel y etiquetas
--   Rueda          acercarse / alejarse (Shift = más rápido)
--   Botón derecho  mantenido + mover el ratón: mirar a tu alrededor
--   W A S D        moverse (Shift = más rápido)     Q / E   bajar / subir
--
-- Cómo se hace desde Lua:
--   local shader = Components:Render():getSceneShaderByLabel("Sepia")   -- por la etiqueta con la que está en la escena
--   shader:setEnabled(true)  /  shader:isEnabled()
--   (getSceneShaderByLabel funciona con shaders de nodos y de código)

-- Efectos de pantalla: etiqueta del shader en la escena y qué hace
local POST = {
    { label = "Sepia",               text = "Grayscale x warm tone" },
    { label = "Duotone",             text = "Grays mapped to two colors (Mix)" },
    { label = "NightVision",         text = "Contrast, green phosphor, vignette and animated grain (Noise)" },
    { label = "Thermal",             text = "Gray mapped to blue > red > yellow (2 x Smoothstep + Mix)" },
    { label = "ChromaticAberration", text = "R and B channels shifted more at the edges (Vec3 Decompose / Compose)" },
    { label = "Underwater",          text = "Per-pixel sine waves on the UV + blue tint" },
    { label = "RetroPixels",         text = "Pixelate + Posterize" },
    { label = "ComicPosterize",      text = "Posterize + Saturation + Contrast" },
    { label = "FilmGrain",           text = "Animated noise added to the image" },
    { label = "Ripple",              text = "Circular waves from the center (UV Radial)" },
    { label = "GhostEcho",           text = "The scene mixed with a copy that sways" },
    { label = "MirrorScreen",        text = "Left half mirrored (Abs on the UV)" },
}

-- Figuras: nombre del objeto en la escena (= nombre del shader) y qué hace el shader
local OBJECTS = {
    { name = "Hologram",        text = "scanlines + Fresnel rim" },
    { name = "NeonPulse",       text = "pulsing glow + rim" },
    { name = "DiagonalStripes", text = "animated stripes (Smoothstep + Mix)" },
    { name = "Lava",            text = "fractal Noise flowing" },
    { name = "Checkerboard",    text = "Floor + Fract on the UV" },
    { name = "NormalColors",    text = "surface normal as color" },
    { name = "FireGlow",        text = "gradient + pulse (existing example)" },
    { name = "HitFlash",        text = "texture flashing to red" },
    { name = "WaterSurface",    text = "texture distorted by waves" },
    { name = "BurnDissolve",    text = "burns away with a glowing edge" },
    { name = "RimLight",        text = "texture + orange Fresnel rim" },
    { name = "ToonOutline",     text = "Posterize + black outline" },
    { name = "ScanLine",        text = "green scanner beam" },
    { name = "PixelatedTexture", text = "Pixelate on the texture UV" },
}

local AUTO_SECONDS = 4.0
local SPIN_DEGREES = 28.0

-- Cámara libre. La posición y el ángulo iniciales son los de la cámara de la escena (ShaderShowcase.json)
local CAM_START_POS   = { x = 0, y = 6.5, z = 24 }
local CAM_START_PITCH = 12.0     -- grados; positivo = mira hacia abajo
local MOUSE_SENS      = 0.12     -- grados por píxel al mirar con el botón derecho
local MOUSE_MAX_STEP  = 80       -- tope de movimiento por frame (evita saltos al capturar el ratón)
local ZOOM_STEP       = 1.6      -- unidades por muesca de la rueda
local MOVE_SPEED      = 9.0      -- unidades por segundo con WASD / QE
local FAST_FACTOR     = 3.0      -- multiplicador con Shift
local MIN_HEIGHT      = 0.4      -- la cámara no baja del suelo

local C_TITLE = Color.new(0.20, 0.85, 1.00, 1.0)
local C_ON    = Color.new(0.20, 1.00, 0.30, 1.0)
local C_DIM   = Color.new(0.65, 0.65, 0.72, 1.0)
local C_KEY   = Color.new(1.00, 0.90, 0.20, 1.0)
local C_TAG   = Color.new(1.00, 1.00, 1.00, 1.0)

local S = {}
local tw

local function reset()
    S.shaders = {}        -- por posición en POST
    S.found = 0
    S.retry = 0.0
    S.index = 0           -- 0 = sin efecto de pantalla
    S.auto = true
    S.timer = 0.0
    S.spin = true
    S.hud = true
    S.objects = {}        -- por posición en OBJECTS
    S.cam = { x = CAM_START_POS.x, y = CAM_START_POS.y, z = CAM_START_POS.z, yaw = 0.0, pitch = CAM_START_PITCH }
    S.looking = false     -- botón derecho mantenido: el ratón mira
    S.seen = false        -- ya se vieron los objetos de la escena
    S.missing = 0         -- frames seguidos sin ellos
    S.dead = false        -- la escena se descargó: este script ya no hace nada
end

-- Si los scripts se paran y se vuelven a lanzar desde el editor (Stop / Play) con esta demo cargada, la copia del script
-- puede sobrevivir a la descarga de su escena: seguiría dibujando su HUD y quitando el teclado a las demás demos.
-- Cuando los objetos de la escena desaparecen tras haberse visto, el script se limpia una vez y se apaga.
local function sceneGone()
    if S.dead then return true end
    if Brakeza:getObjectByName(OBJECTS[1].name) ~= nil then S.seen = true; S.missing = 0; return false end
    if not S.seen then return false end
    S.missing = S.missing + 1
    if S.missing < 30 then return false end
    S.dead = true
    onEnd()
    return true
end

local function resolveShaders()
    local render = Components:Render()
    local found = 0
    for i, def in ipairs(POST) do
        if S.shaders[i] == nil then
            S.shaders[i] = render:getSceneShaderByLabel(def.label)
        end
        if S.shaders[i] ~= nil then found = found + 1 end
    end
    S.found = found
end

-- Solo uno encendido a la vez
local function applyEffect()
    for i = 1, #POST do
        if S.shaders[i] ~= nil then S.shaders[i]:setEnabled(i == S.index) end
    end
end

local function goTo(index)
    local count = #POST + 1                      -- + la posición 0 (sin efecto)
    S.index = (index % count)
    S.timer = 0.0
    applyEffect()
end

local function spinObjects(dt)
    for i, def in ipairs(OBJECTS) do
        if S.objects[i] == nil or S.objects[i]:isRemoved() then
            S.objects[i] = Brakeza:getObjectByName(def.name)
        end
        local obj = S.objects[i]
        if obj ~= nil then
            local delta = M3.new()
            delta:setMatrixRotationForEulerAngles(0, SPIN_DEGREES * dt, 0)   -- los ángulos van en grados
            obj:setRotation(delta * obj:getRotation())
        end
    end
end

-- Cámara libre: rueda = zoom, botón derecho + ratón = mirar, WASD/QE = moverse
local function updateCamera(input, dt)
    local c = S.cam
    local fast = (input:isCharPressed("Left Shift") or input:isCharPressed("Right Shift")) and FAST_FACTOR or 1.0

    -- Mirar: con el botón derecho mantenido el cursor queda capturado y el giro no se acaba en el borde de la ventana
    local right = input:isRightMouseButtonPressed()
    if right ~= S.looking then
        S.looking = right
        pcall(function() input:setRelativeMouseMode(right) end)
    end
    if S.looking and input:isMouseMotion() then
        local dx = math.max(-MOUSE_MAX_STEP, math.min(MOUSE_MAX_STEP, input:getMouseMotionXRel()))
        local dy = math.max(-MOUSE_MAX_STEP, math.min(MOUSE_MAX_STEP, input:getMouseMotionYRel()))
        c.yaw   = c.yaw + dx * MOUSE_SENS
        c.pitch = math.max(-89, math.min(89, c.pitch + dy * MOUSE_SENS))   -- ratón abajo = mirar abajo
    end

    -- Dirección de la mirada y su eje lateral (horizontal)
    local yr, pr = math.rad(c.yaw), math.rad(c.pitch)
    local fx, fy, fz = math.sin(yr) * math.cos(pr), -math.sin(pr), -math.cos(yr) * math.cos(pr)
    local rx, rz = math.cos(yr), math.sin(yr)

    -- Zoom con la rueda: avanza o retrocede por donde miras
    local wheel = input:getMouseWheelY()
    if wheel ~= 0 then
        local d = wheel * ZOOM_STEP * fast
        c.x, c.y, c.z = c.x + fx * d, c.y + fy * d, c.z + fz * d
    end

    -- Teclado
    local step = MOVE_SPEED * fast * dt
    if input:isCharPressed("W") then c.x, c.y, c.z = c.x + fx * step, c.y + fy * step, c.z + fz * step end
    if input:isCharPressed("S") then c.x, c.y, c.z = c.x - fx * step, c.y - fy * step, c.z - fz * step end
    if input:isCharPressed("D") then c.x, c.z = c.x + rx * step, c.z + rz * step end
    if input:isCharPressed("A") then c.x, c.z = c.x - rx * step, c.z - rz * step end
    if input:isCharPressed("E") then c.y = c.y + step end
    if input:isCharPressed("Q") then c.y = c.y - step end
    c.y = math.max(MIN_HEIGHT, c.y)

    local camera = Components:Camera():getCamera()
    camera:setPosition(Vertex3D.new(c.x, c.y, c.z))
    camera:setRotationFromEulerAngles(c.pitch, c.yaw, 0)
end

function onStart()
    tw = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
    reset()
    -- El teclado del motor movería la cámara con las flechas (las del cambio de efecto): se gestiona aquí
    pcall(function() Components:Input():setKeyboardEnabled(false) end)
end

function onUpdate()
    if sceneGone() then return end
    local ok, err = pcall(function()
        local input = Components:Input()
        local dt = Brakeza:getDeltaTime()

        -- Los shaders de escena cargan en segundo plano: se resuelven los que falten
        if S.found < #POST then
            S.retry = S.retry - dt
            if S.retry <= 0 then
                S.retry = 0.25
                resolveShaders()
                applyEffect()
            end
        end

        if input:isCharFirstEventDown("Right") then goTo(S.index + 1) end
        if input:isCharFirstEventDown("Left")  then goTo(S.index - 1) end
        if input:isCharFirstEventDown("0")     then goTo(0) end
        if input:isCharFirstEventDown("Space") then S.auto = not S.auto; S.timer = 0.0 end
        if input:isCharFirstEventDown("R")     then S.spin = not S.spin end
        if input:isCharFirstEventDown("H")     then S.hud = not S.hud end

        if S.auto and S.found == #POST then
            S.timer = S.timer + dt
            if S.timer >= AUTO_SECONDS then goTo(S.index + 1) end
        end

        if S.spin then spinObjects(dt) end

        updateCamera(input, dt)
    end)
    if not ok then print("[ShaderShowcase] ERROR: " .. tostring(err)) end
end

function postUpdate()
    if tw == nil or sceneGone() then return end
    pcall(function()
        if not S.hud then
            tw:writeTextTTFAutoSize(20, 20, "H - show panel", C_DIM, 0.5)
            return
        end

        local X, Y, LH = 20, 20, 24
        tw:writeTextTTFAutoSize(X, Y, "Node shader showcase", C_TITLE, 0.80); Y = Y + LH + 8

        local effect = (S.index == 0) and "none" or POST[S.index].label
        tw:writeTextTTFAutoSize(X, Y, string.format("Screen effect %d/%d: %s", S.index, #POST, effect),
            S.index == 0 and C_DIM or C_ON, 0.65); Y = Y + LH + 2
        if S.index > 0 then
            tw:writeTextTTFAutoSize(X, Y, POST[S.index].text, C_DIM, 0.45); Y = Y + LH
        end
        if S.found < #POST then
            tw:writeTextTTFAutoSize(X, Y, string.format("loading shaders... %d/%d", S.found, #POST), C_DIM, 0.45); Y = Y + LH
        end
        Y = Y + 6
        tw:writeTextTTFAutoSize(X, Y, "Left/Right effect   0 none   Space auto " .. (S.auto and "ON" or "off") ..
            "   R spin " .. (S.spin and "ON" or "off") .. "   H hide", C_KEY, 0.45); Y = Y + LH
        tw:writeTextTTFAutoSize(X, Y, "Wheel zoom   hold right button + mouse: look   WASD move   Q/E down/up   Shift faster",
            C_DIM, 0.45)

        -- Etiqueta sobre cada figura, proyectada a pantalla
        local cam = Components:Camera()
        local w, h = Components:Window():getWidthRender(), Components:Window():getHeightRender()
        for i, def in ipairs(OBJECTS) do
            local obj = S.objects[i]
            if obj == nil then obj = Brakeza:getObjectByName(def.name); S.objects[i] = obj end
            if obj ~= nil then
                local p = obj:getPosition()
                local sx, sy, z = cam:worldToScreenXYZ(p.x, p.y + 2.1, p.z, w, h)
                if z > 0 then
                    tw:writeTextTTFAutoSize(sx - #def.name * 3.3, sy, def.name, C_TAG, 0.42)
                end
            end
        end
    end)
end

function onEnd()
    -- Devolver el teclado y el ratón al motor
    pcall(function() Components:Input():setKeyboardEnabled(true) end)
    if S.looking then pcall(function() Components:Input():setRelativeMouseMode(false) end) end
    -- Dejar la cadena como estaba al cargar: todos los efectos apagados
    pcall(function()
        for i = 1, #POST do
            if S.shaders[i] ~= nil then S.shaders[i]:setEnabled(false) end
        end
    end)
    S.shaders, S.objects = {}, {}
end
