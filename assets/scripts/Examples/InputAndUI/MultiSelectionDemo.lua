-- MultiSelectionDemo.lua
-- Demonstrates the multi-selection API exposed to Lua via ComponentRender.
--
-- How to use:
--   1. Attach as a Global Script on the scene and press Play (F1).
--   2. Click objects in the viewport to select them.
--      CTRL + Click  → toggle individual object into/out of the group.
--      CTRL + Drag   → rectangle-select multiple objects at once.
--   3. Use the keyboard shortcuts shown on screen to operate on the group.
--
-- API covered:
--   render:getSelectedObject()       -> Object3D | nil   (only when exactly 1 selected)
--   render:getSelectedObjects()      -> table{Object3D}  (all selected, always safe)
--   render:hasMultipleSelected()     -> bool
--   render:isObjectInSelection(obj)  -> bool
--   render:addToSelection(obj)       -> void11
--   render:removeFromSelection(obj)  -> void
--   render:clearSelection()          -> void
--   render:setSelectedObject(obj)    -> void             (replace group with one object)

local PAD_X    = 20
local PAD_Y    = 20
local LINE_H   = 24
local MAX_LIST = 12

-- ──────────────────────────────────────────────────────────────────────────────
-- EFECTOS DE SELECCIÓN
-- ──────────────────────────────────────────────────────────────────────────────
-- Los seleccionados se distinguen con cuatro cosas a la vez:
--   1. contorno naranja del motor            object:setHighlight(...) / clearHighlight()
--   2. anillo pulsante en el suelo debajo    render:drawGroundCircle(obj, r,g,b,a, radio, grosor)
--   3. número de orden sobre cada objeto     camera:worldToScreenXYZ + TextWriter
--   4. con 2 o más: líneas hasta el centro del grupo, un anillo que lo abarca y una marca en el centro
-- (Con el exe en marcha el motor no dibuja su contorno verde de selección del editor: sin esto no se vería nada.)

local fxTime      = 0.0
local centerMarker = nil      -- objeto auxiliar en el centro del grupo (para dibujar sus anillos)
local highlighted = {}        -- [nombre] = objeto al que se le puso setHighlight (las claves son nombres: el userdata cambia)

local C_SINGLE = { 0.30, 1.00, 0.30 }     -- anillo con 1 seleccionado
local C_GROUP  = { 0.25, 0.75, 1.00 }     -- anillo con varios
local C_CENTER = { 1.00, 0.85, 0.20 }     -- centro y anillo del grupo

local function alive(o)
    if o == nil then return false end
    local ok, removed = pcall(function() return o:isRemoved() end)
    return ok and not removed
end

-- Contorno nativo: se pone a los seleccionados y se quita a los que dejan de estarlo
local function syncHighlights(selected)
    local now = {}
    for _, o in ipairs(selected) do
        local name = o:getName()
        now[name] = true
        if highlighted[name] == nil then
            pcall(function() o:setHighlight(1.0, 0.55, 0.1, 1.0) end)
            highlighted[name] = o
        end
    end
    for name, o in pairs(highlighted) do
        if not now[name] then
            if alive(o) then pcall(function() o:clearHighlight() end) end
            highlighted[name] = nil
        end
    end
end

local function clearAllHighlights()
    for name, o in pairs(highlighted) do
        if alive(o) then pcall(function() o:clearHighlight() end) end
        highlighted[name] = nil
    end
end

local function drawSelectionEffects(selected)
    local count = #selected
    syncHighlights(selected)
    if count == 0 then return end

    local pulse = 0.5 + 0.5 * math.sin(fxTime * 4.0)          -- 0..1
    local base  = count > 1 and C_GROUP or C_SINGLE

    -- 2. anillo bajo cada seleccionado (radio según su escala, late un poco)
    local cx, cy, cz = 0, 0, 0
    for _, o in ipairs(selected) do
        local p = o:getPosition()
        cx, cy, cz = cx + p.x, cy + p.y, cz + p.z
        local r = math.max(0.8, (tonumber(o:getScale()) or 1.0) * 0.95) * (1.0 + 0.08 * pulse)
        render:drawGroundCircle(o, base[1], base[2], base[3], 0.55 + 0.4 * pulse, r, 0.12)
    end

    -- 3. número de orden sobre cada objeto
    local cam = Components:Camera()
    local win = Components:Window()
    local w, h = win:getWidth(), win:getHeight()
    for i, o in ipairs(selected) do
        local p = o:getPosition()
        local sx, sy, sz = cam:worldToScreenXYZ(p.x, p.y + 1.9, p.z, w, h)
        if sz > 0 then
            tw:writeTextTTFAutoSize(sx - 8, sy, tostring(i), Color.new(base[1], base[2], base[3], 1), 1.3)
        end
    end

    -- 4. centro del grupo: marca, líneas a cada miembro y un anillo que los abarca
    if count > 1 then
        cx, cy, cz = cx / count, cy / count, cz / count
        local center = Vertex3D.new(cx, cy + 0.3, cz)
        local lineColor = Color.new(C_CENTER[1], C_CENTER[2], C_CENTER[3], 1.0)
        local far = 0
        for _, o in ipairs(selected) do
            local p = o:getPosition()
            render:DrawLine(center, Vertex3D.new(p.x, p.y + 0.3, p.z), lineColor)
            local dx, dz = p.x - cx, p.z - cz
            far = math.max(far, math.sqrt(dx * dx + dz * dz))
        end

        -- Los anillos se dibujan alrededor de un objeto: se usa una esfera pequeña colocada en el centro del grupo,
        -- que además es la marca visible del centro (ObjectFactory.Object3D no sirve: no queda registrado en la escena)
        if not alive(centerMarker) then
            centerMarker = ObjectFactory.Mesh3D("../assets/models/basic/icosphere.fbx", Vertex3D.new(cx, 0.1, cz))
            centerMarker:setName("MultiSelection_Center")
            centerMarker:setScale(0.35)
            centerMarker:setSelectable(false)
        end
        centerMarker:setPosition(Vertex3D.new(cx, 0.1, cz))
        render:drawGroundCircle(centerMarker, C_CENTER[1], C_CENTER[2], C_CENTER[3], 0.9, 0.35, 0.12)
        render:drawGroundCircle(centerMarker, C_CENTER[1], C_CENTER[2], C_CENTER[3], 0.35 + 0.35 * pulse, far + 1.6, 0.08)
    end
end

-- ──────────────────────────────────────────────────────────────────────────────

function onStart()
    tw     = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    render = Components:Render()
    input  = Components:Input()

    nudgeStep = 0.05   -- world-units per frame applied while key is held

    print("[MultiSelectionDemo] loaded — see on-screen help for controls")
end

-- ──────────────────────────────────────────────────────────────────────────────

function onUpdate()
    render = Components:Render()
    input  = Components:Input()

    fxTime = fxTime + Brakeza:getDeltaTime()

    handleInput()
    drawSelectionEffects(render:getSelectedObjects())
    drawHUD()
end

-- ──────────────────────────────────────────────────────────────────────────────
-- INPUT HANDLING
-- ──────────────────────────────────────────────────────────────────────────────

function handleInput()
    -- the big floor is only scenery: a click on it or a rectangle that overlaps it must not select it
    for _, o in ipairs(render:getSelectedObjects()) do
        if o:getName() == "Ground" then render:removeFromSelection(o) end
    end

    local selected = render:getSelectedObjects()
    local count    = #selected

    -- R (first event) → clear entire selection
    if input:isCharFirstEventDown("R") then
        render:clearSelection()
        print("[MultiSelectionDemo] selection cleared")
        return   -- nothing else to do this frame
    end

    -- X (first event) → remove the last object from the group (doesn't delete it)
    if input:isCharFirstEventDown("X") then
        if count > 0 then
            local last = selected[count]
            render:removeFromSelection(last)
            print("[MultiSelectionDemo] removed '" .. last:getName() .. "' from group")
        end
        return
    end

    -- P (first event) → dump full report to console
    if input:isCharFirstEventDown("P") then
        printSelectionReport(selected)
        return
    end

    -- WASD / Q-Z → nudge ALL selected objects while the key is held
    if count == 0 then return end

    local nudge = Vertex3D.new(0, 0, 0)
    local dt    = Brakeza:getDeltaTime()
    local step  = nudgeStep / dt  -- frame-rate independent

    if input:isCharPressed("W") then nudge.z = nudge.z - step end
    if input:isCharPressed("S") then nudge.z = nudge.z + step end
    if input:isCharPressed("A") then nudge.x = nudge.x - step end
    if input:isCharPressed("D") then nudge.x = nudge.x + step end
    if input:isCharPressed("Q") then nudge.y = nudge.y + step end
    if input:isCharPressed("Z") then nudge.y = nudge.y - step end

    if nudge.x ~= 0 or nudge.y ~= 0 or nudge.z ~= 0 then
        for i = 1, count do
            selected[i]:addToPosition(nudge:getScaled(dt))
        end
    end
end

function printSelectionReport(selected)
    local count = #selected
    print("──────────────────────────────────────────────")
    print("[MultiSelectionDemo] " .. count .. " object(s) selected")
    for i = 1, count do
        local obj = selected[i]
        local pos = obj:getPosition()
        print(string.format("  [%d] %-30s  pos=(%.2f, %.2f, %.2f)",
            i, obj:getName(), pos.x, pos.y, pos.z))
    end
    print("──────────────────────────────────────────────")
end

-- ──────────────────────────────────────────────────────────────────────────────
-- HUD / DEBUG OVERLAY
-- ──────────────────────────────────────────────────────────────────────────────

function drawHUD()
    local selected = render:getSelectedObjects()
    local count    = #selected
    local multi    = render:hasMultipleSelected()

    local x = PAD_X
    local y = PAD_Y

    -- Title
    tw:writeTextTTFAutoSize(x, y, "[ Multi-Selection Demo ]", Color.new(1, 0.8, 0, 1), 0.85)
    y = y + LINE_H + 6

    -- Mode badge
    local modeLabel, modeColor
    if count == 0 then
        modeLabel = "NONE"
        modeColor = Color.new(0.45, 0.45, 0.45, 1)
    elseif count == 1 then
        modeLabel = "SINGLE"
        modeColor = Color.new(0.3, 1, 0.3, 1)
    else
        modeLabel = "MULTI"
        modeColor = Color.new(0.3, 0.7, 1, 1)
    end

    tw:writeTextTTFAutoSize(x, y,
        "Mode: " .. modeLabel .. "   |   Selected: " .. count,
        modeColor, 0.7)
    y = y + LINE_H + 4

    -- Single-object detail
    if count == 1 then
        local single = render:getSelectedObject()   -- returns nil when count != 1
        if single then
            local pos = single:getPosition()
            tw:writeTextTTFAutoSize(x, y,
                "Name: " .. single:getName(), Color.new(0.9, 0.9, 0.9, 1), 0.7)
            y = y + LINE_H
            tw:writeTextTTFAutoSize(x, y,
                string.format("Pos:  (%.3f,  %.3f,  %.3f)", pos.x, pos.y, pos.z),
                Color.new(0.7, 0.7, 0.7, 1), 0.7)
            y = y + LINE_H + 4
        end
    end

    -- Object list (multi-select)
    if count > 1 then
        tw:writeTextTTFAutoSize(x, y, "Objects in group:", Color.new(0.8, 0.8, 0.8, 1), 0.7)
        y = y + LINE_H

        local limit   = math.min(count, MAX_LIST)
        local colObj  = Color.new(0.55, 0.9, 1, 1)
        local colPos  = Color.new(0.6, 0.6, 0.6, 1)

        for i = 1, limit do
            local obj = selected[i]
            local pos = obj:getPosition()
            tw:writeTextTTFAutoSize(x + 10, y,
                string.format("%2d.  %-28s  (%.1f, %.1f, %.1f)",
                    i, obj:getName(), pos.x, pos.y, pos.z),
                i % 2 == 0 and colPos or colObj, 0.65)
            y = y + LINE_H
        end

        if count > MAX_LIST then
            tw:writeTextTTFAutoSize(x + 10, y,
                "... and " .. (count - MAX_LIST) .. " more",
                Color.new(0.5, 0.5, 0.5, 1), 0.65)
            y = y + LINE_H
        end
        y = y + 4
    end

    -- ── Controls legend: bottom-left, in two columns, clear of the objects (upper half) and of the demo browser's info panel (right) ──
    local winH  = Components:Window():getHeightRender()
    local lx    = PAD_X
    local rx    = PAD_X + 400
    local ly    = (winH > 0 and winH or 720) - 70 - LINE_H * 10 - 12     -- 70 px: the browser's bottom bar

    local cTitle = Color.new(1.0,  0.85, 0.2,  1)
    local cLine  = Color.new(0.75, 0.75, 0.75, 1)
    local FS     = 0.58

    tw:writeTextTTFAutoSize(lx, ly,                "[ CONTROLS ]",                   cTitle, FS)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 1,   "Click          Select single",    cLine,  FS)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 2,   "CTRL+Click     Toggle add/remove",cLine,  FS)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 3,   "CTRL+Drag      Rectangle select", cLine,  FS)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 4,   "W / S          Move +Z / -Z",     cLine,  FS)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 5,   "A / D          Move -X / +X",     cLine,  FS)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 6,   "O / Z          Move +Y / -Y",     cLine,  FS)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 7,   "X              Remove last",       cLine,  FS)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 8,   "R              Clear selection",   cLine,  FS)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 9,   "P              Print to console",  cLine,  FS)

    -- What the effects in the scene mean
    tw:writeTextTTFAutoSize(rx, ly,                "[ WHAT YOU SEE ]",                cTitle, FS)
    tw:writeTextTTFAutoSize(rx, ly + LINE_H * 1,   "Orange outline  Selected",        Color.new(1.0, 0.55, 0.1, 1), FS)
    tw:writeTextTTFAutoSize(rx, ly + LINE_H * 2,   "Green ring      Single selection",Color.new(0.3, 1.0, 0.3, 1), FS)
    tw:writeTextTTFAutoSize(rx, ly + LINE_H * 3,   "Blue ring       Part of a group", Color.new(0.25, 0.75, 1.0, 1), FS)
    tw:writeTextTTFAutoSize(rx, ly + LINE_H * 4,   "Yellow          Group centre",    Color.new(1.0, 0.85, 0.2, 1), FS)
end

-- ──────────────────────────────────────────────────────────────────────────────

function onEnd()
    clearAllHighlights()
    if alive(centerMarker) then centerMarker:setRemoved(true) end
    centerMarker = nil
    render:clearSelection()
    print("[MultiSelectionDemo] unloaded")
end
