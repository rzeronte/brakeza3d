-- MouseIconChangeDemo.lua
-- Demo del cursor del ratón: el cursor cambia según el objeto que hay debajo, igual que hace el RTS.
--
-- Cómo funciona (3 piezas):
--   1. window:scheduleHoverPick(x, y)   pide al motor qué objeto hay bajo el ratón; la respuesta llega un frame
--                                       después, sin parar la GPU (lectura asíncrona del buffer de picking)
--   2. window:getHoverPickID()          id de lo que había bajo el ratón en la última petición (0 = nada)
--      render:getObjectByPickingID(id)  del id al objeto
--   3. window:LoadCursorImage(png)      cambia el cursor del sistema por esa imagen (24x24)
-- Se pide el picking solo cuando el ratón se mueve, y el cursor solo se cambia cuando cambia de estado.
--
-- CONTROLS
--   Mouse over the objects    the cursor changes by object (see CURSOR_BY_OBJECT below)
--   1 - 4                     force one cursor (select / move / attack / acquire)
--   0                         automatic again
--   H                         show / hide the panel

-- Iconos propios de esta demo (assets/icons/cursors/, PNG de 24x24): se pueden cambiar sin afectar al RTS, que tiene los suyos en assets/icons/RTS/
local ICONS = {
    default = "../assets/icons/mouse.png",
    select  = "../assets/icons/cursors/cursor_select.png",
    move    = "../assets/icons/cursors/cursor_move.png",
    attack  = "../assets/icons/cursors/cursor_attack.png",
    acquire = "../assets/icons/cursors/cursor_acquire.png",
}

-- Nombre del objeto de la escena -> estado de cursor. Lo que no está aquí deja el cursor por defecto.
local CURSOR_BY_OBJECT = {
    Target_Select  = "select",
    Target_Move    = "move",
    Target_Attack  = "attack",
    Target_Acquire = "acquire",
}

local FORCE_KEYS = { ["1"] = "select", ["2"] = "move", ["3"] = "attack", ["4"] = "acquire" }

local C_TITLE = Color.new(0.20, 0.85, 1.00, 1.0)
local C_WHITE = Color.new(1.00, 1.00, 1.00, 1.0)
local C_DIM   = Color.new(0.55, 0.55, 0.60, 1.0)
local C_ON    = Color.new(0.20, 1.00, 0.30, 1.0)
local C_KEY   = Color.new(1.00, 0.90, 0.20, 1.0)

local S = { current = nil, forced = nil, hovered = "(nothing)", showHUD = true, lastMX = -1, lastMY = -1 }
local win, render, input, tw

local function apply(state)
    if state == S.current then return end      -- LoadCursorImage crea un cursor SDL nuevo: no se repite sin necesidad
    win:LoadCursorImage(ICONS[state] or ICONS.default)
    S.current = state
end

function onStart()
    win    = Components:Window()
    render = Components:Render()
    input  = Components:Input()
    tw     = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
    S.current, S.forced, S.hovered, S.showHUD, S.lastMX, S.lastMY = nil, nil, "(nothing)", true, -1, -1
    apply("default")
end

local function hoveredObjectName()
    local id = win:getHoverPickID()
    if id == 0 then return nil end
    local obj = render:getObjectByPickingID(id)
    if obj == nil then return nil end
    return obj:getName()
end

function onUpdate()
    local ok, err = pcall(function()
        win, render, input = Components:Window(), Components:Render(), Components:Input()

        for key, state in pairs(FORCE_KEYS) do
            if input:isCharFirstEventDown(key) then S.forced = state end
        end
        if input:isCharFirstEventDown("0") then S.forced = nil end
        if input:isCharFirstEventDown("H") then S.showHUD = not S.showHUD end

        -- Pedir el picking solo si el ratón se ha movido
        local mx, my = input:getRawMouseX(), input:getRawMouseY()
        if mx ~= S.lastMX or my ~= S.lastMY then
            S.lastMX, S.lastMY = mx, my
            win:scheduleHoverPick(mx, my)
        end

        local name = hoveredObjectName()
        S.hovered = name or "(nothing)"
        apply(S.forced or (name and CURSOR_BY_OBJECT[name]) or "default")
    end)
    if not ok then print("[MouseIconChangeDemo] ERROR: " .. tostring(err)) end
end

function postUpdate()
    if tw == nil then return end
    pcall(function()
        if not S.showHUD then
            tw:writeTextTTFAutoSize(20, 20, "H - show panel", C_DIM, 0.5)
            return
        end
        local X, Y, LH = 20, 20, 26
        tw:writeTextTTFAutoSize(X, Y, "Mouse cursor", C_TITLE, 0.80);                           Y = Y + LH + 8
        tw:writeTextTTFAutoSize(X, Y, "Move the mouse over the objects", C_DIM, 0.5);         Y = Y + LH + 6
        tw:writeTextTTFAutoSize(X, Y, "Under the mouse: " .. S.hovered, C_WHITE, 0.55);       Y = Y + LH
        tw:writeTextTTFAutoSize(X, Y, "Cursor: " .. tostring(S.current) ..
            (S.forced and "  (forced)" or "  (automatic)"), C_ON, 0.55);                       Y = Y + LH + 8
        tw:writeTextTTFAutoSize(X, Y, "1-4 force a cursor    0 automatic    H hide panel", C_KEY, 0.45)
    end)
end

function onEnd()
    -- Devolver el cursor normal al salir de la demo
    pcall(function() Components:Window():LoadCursorImage(ICONS.default) end)
    S.current = nil
end
