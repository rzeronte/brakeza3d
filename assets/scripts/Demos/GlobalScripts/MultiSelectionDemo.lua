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

    handleInput()
    drawHUD()
end

-- ──────────────────────────────────────────────────────────────────────────────
-- INPUT HANDLING
-- ──────────────────────────────────────────────────────────────────────────────

function handleInput()
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
    tw:writeTextTTFAutoSize(x, y, "[ Multi-Selection Demo ]", Color.new(1, 0.8, 0, 1), 1.1)
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
        modeColor, 1.0)
    y = y + LINE_H + 4

    -- Single-object detail
    if count == 1 then
        local single = render:getSelectedObject()   -- returns nil when count != 1
        if single then
            local pos = single:getPosition()
            tw:writeTextTTFAutoSize(x, y,
                "Name: " .. single:getName(), Color.new(0.9, 0.9, 0.9, 1), 1.0)
            y = y + LINE_H
            tw:writeTextTTFAutoSize(x, y,
                string.format("Pos:  (%.3f,  %.3f,  %.3f)", pos.x, pos.y, pos.z),
                Color.new(0.7, 0.7, 0.7, 1), 1.0)
            y = y + LINE_H + 4
        end
    end

    -- Object list (multi-select)
    if count > 1 then
        tw:writeTextTTFAutoSize(x, y, "Objects in group:", Color.new(0.8, 0.8, 0.8, 1), 1.0)
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
                i % 2 == 0 and colPos or colObj, 0.9)
            y = y + LINE_H
        end

        if count > MAX_LIST then
            tw:writeTextTTFAutoSize(x + 10, y,
                "... and " .. (count - MAX_LIST) .. " more",
                Color.new(0.5, 0.5, 0.5, 1), 0.9)
            y = y + LINE_H
        end
        y = y + 4
    end

    -- ── Controls legend (right side) ──────────────────────────────────────
    local winW  = Components:Window():getWidthRender()
    local lx    = (winW > 0 and winW or 1280) - 700
    local ly    = PAD_Y

    local cTitle = Color.new(1.0,  0.85, 0.2,  1)
    local cLine  = Color.new(0.75, 0.75, 0.75, 1)

    tw:writeTextTTFAutoSize(lx, ly,                "[ CONTROLS ]",                   cTitle, 1.0)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 1,   "Click          Select single",    cLine,  1.0)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 2,   "CTRL+Click     Toggle add/remove",cLine,  1.0)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 3,   "CTRL+Drag      Rectangle select", cLine,  1.0)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 4,   "W / S          Move +Z / -Z",     cLine,  1.0)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 5,   "A / D          Move -X / +X",     cLine,  1.0)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 6,   "Q / Z          Move +Y / -Y",     cLine,  1.0)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 7,   "X              Remove last",       cLine,  1.0)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 8,   "R              Clear selection",   cLine,  1.0)
    tw:writeTextTTFAutoSize(lx, ly + LINE_H * 9,   "P              Print to console",  cLine,  1.0)
end

-- ──────────────────────────────────────────────────────────────────────────────

function onEnd()
    render:clearSelection()
    print("[MultiSelectionDemo] unloaded")
end
