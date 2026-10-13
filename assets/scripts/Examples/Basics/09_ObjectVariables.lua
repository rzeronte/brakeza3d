-- ============================================================================
-- Tutorial 09: Reading Variables of Another Object
-- Type: Object Script
-- ============================================================================
-- A script can read the variables another object keeps in its own scripts:
--   local other = Brakeza:getObjectByName("MyObject")
--   other:getLocalScriptVar("count")
--
-- The reader prints the values once a second (open the Logs / Console window to see them).
-- Variables live in the .json file of the script (Variables window of the editor) and can be Vertex3D, int, float, string...
-- The object that owns them is "MyObject"; its script is 09_ObjectVariablesSource.lua.
-- ============================================================================

local timer = 0.0
local textWriter = nil
local info = "Looking for 'MyObject'..."

function onUpdate()
    textWriter = textWriter or ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)

    timer = timer + Brakeza:getDeltaTime()
    if timer >= 1.0 then
        timer = timer - 1.0

        local other = Brakeza:getObjectByName("MyObject")
        if other == nil then
            info = "Object 'MyObject' not found (yet)"
        else
            local offset = other:getLocalScriptVar("offset")
            local count = other:getLocalScriptVar("count")
            if offset ~= nil and count ~= nil then
                info = string.format("%s: offset = (%.1f, %.1f, %.1f)   count = %d",
                    other:getName(), offset.x, offset.y, offset.z, count)
                print("[09_ObjectVariables] " .. info)
            end
        end
    end

    -- también en pantalla, no solo en la consola
    local screenW = Components:Window():getWidth()
    textWriter:writeTextTTFAutoSize(screenW / 2 - 200, 100, "=== READING VARIABLES OF ANOTHER OBJECT ===", Color.new(1, 1, 0, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW / 2 - 200, 140, info, Color.new(0, 1, 0, 1), 1.0)
end
