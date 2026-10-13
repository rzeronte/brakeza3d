-- PostProcessingDemo.lua
-- Demo interactivo de la cadena de postproceso: cada tecla activa o desactiva uno de los 8 shaders de la escena.
--
-- Cada shader (Quarter01..08) pinta un trozo distinto de la pantalla (una cuadrícula de 4 columnas x 2 filas).
-- Las teclas siguen esa cuadrícula:
--      1 2 3 4        fila de arriba
--      5 6 7 8        fila de abajo
-- Se ejecutan en orden, 1 -> 8: cada uno recibe la imagen que dejó el anterior (buffers ping-pong).
--
-- CONTROLS
--   1 - 8      activar / desactivar ese shader
--   0          apagarlos todos
--   9          encenderlos todos
--   H          mostrar / ocultar el panel
--
-- Cómo se hace desde Lua:
--   local shader = Components:Render():getSceneShaderByLabel("QuarterRed")   -- por el nombre con el que está en la escena
--   shader:setEnabled(false)  /  shader:isEnabled()

-- Etiqueta del shader en la escena, color que pinta (solo para el panel) y celda de la pantalla (columna, fila)
local SHADERS = {
    { label = "QuarterRed",     name = "Red",           col = 1, row = 1 },
    { label = "QuarterGreen",   name = "Green",         col = 2, row = 1 },
    { label = "QuarterBlue",    name = "Blue",          col = 3, row = 1 },
    { label = "QuarterYellow",  name = "Yellow",        col = 4, row = 1 },
    { label = "QuarterCyan",    name = "Cyan",          col = 1, row = 2 },
    { label = "QuarterMagenta", name = "Magenta",       col = 2, row = 2 },
    { label = "QuarterWhite",   name = "Inverted grey", col = 3, row = 2 },   -- este no pinta blanco: invierte 0.5 - color
    { label = "QuarterOrange",  name = "Orange",        col = 4, row = 2 },
}

local C_TITLE = Color.new(0.20, 0.85, 1.00, 1.0)
local C_ON    = Color.new(0.20, 1.00, 0.30, 1.0)
local C_OFF   = Color.new(0.90, 0.35, 0.30, 1.0)
local C_DIM   = Color.new(0.60, 0.60, 0.65, 1.0)
local C_KEY   = Color.new(1.00, 0.90, 0.20, 1.0)

local S = { shaders = {}, found = 0, retry = 0.0, showHUD = true }
local render, input, tw

local function resolveShaders()
    local found = 0
    for i, def in ipairs(SHADERS) do
        if S.shaders[i] == nil then
            S.shaders[i] = render:getSceneShaderByLabel(def.label)
        end
        if S.shaders[i] ~= nil then found = found + 1 end
    end
    S.found = found
end

local function setAll(on)
    for i = 1, #SHADERS do
        if S.shaders[i] ~= nil then S.shaders[i]:setEnabled(on) end
    end
end

function onStart()
    render = Components:Render()
    input  = Components:Input()
    tw     = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
    S.shaders, S.found, S.retry, S.showHUD = {}, 0, 0.0, true
end

function onUpdate()
    local ok, err = pcall(function()
        render, input = Components:Render(), Components:Input()

        -- Los shaders de escena cargan de forma asíncrona: se van resolviendo los que falten
        if S.found < #SHADERS then
            S.retry = S.retry - Brakeza:getDeltaTime()
            if S.retry <= 0 then
                S.retry = 0.25
                resolveShaders()
            end
        end

        for i = 1, #SHADERS do
            if input:isCharFirstEventDown(tostring(i)) and S.shaders[i] ~= nil then
                S.shaders[i]:setEnabled(not S.shaders[i]:isEnabled())
            end
        end
        if input:isCharFirstEventDown("0") then setAll(false) end
        if input:isCharFirstEventDown("9") then setAll(true) end
        if input:isCharFirstEventDown("H") then S.showHUD = not S.showHUD end
    end)
    if not ok then print("[PostProcessingDemo] ERROR: " .. tostring(err)) end
end

function postUpdate()
    if tw == nil then return end
    pcall(function()
        if not S.showHUD then
            tw:writeTextTTFAutoSize(20, 20, "H - show panel", C_DIM, 0.5)
            return
        end

        local active = 0
        for i = 1, #SHADERS do
            if S.shaders[i] ~= nil and S.shaders[i]:isEnabled() then active = active + 1 end
        end

        local X, Y, LH = 20, 20, 24
        tw:writeTextTTFAutoSize(X, Y, "Post-processing chain", C_TITLE, 0.80);                     Y = Y + LH + 8
        tw:writeTextTTFAutoSize(X, Y, string.format("%d of %d shaders active%s", active, #SHADERS,
            S.found < #SHADERS and "  (loading...)" or ""), C_DIM, 0.5);                           Y = Y + LH + 6

        -- La cuadrícula del panel repite la de la pantalla: la tecla N toca la celda N
        for row = 1, 2 do
            for col = 1, 4 do
                local i = (row - 1) * 4 + col
                local def = SHADERS[i]
                local sh = S.shaders[i]
                local on = sh ~= nil and sh:isEnabled()
                tw:writeTextTTFAutoSize(X + (col - 1) * 215, Y,
                    string.format("[%d] %-13s %s", i, def.name, sh == nil and "..." or (on and "ON" or "off")),
                    sh == nil and C_DIM or (on and C_ON or C_OFF), 0.5)
            end
            Y = Y + LH
        end

        Y = Y + 8
        tw:writeTextTTFAutoSize(X, Y, "1-8 toggle    0 all off    9 all on    H hide panel", C_KEY, 0.45);       Y = Y + LH
        tw:writeTextTTFAutoSize(X, Y, "Each shader changes one cell of the screen; they run in order 1 to 8", C_DIM, 0.45)
    end)
end

function onEnd()
    -- Dejar la cadena como estaba: todos encendidos
    pcall(setAll, true)
    S.shaders = {}
end
