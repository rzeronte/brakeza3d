-- ============================================================================
-- Tutorial 09: Object Variables: the object that owns them
-- Type: Object Script
-- ============================================================================
-- This script only HOLDS two variables, declared in its .json file:
--   offset  (Vertex3D)     count  (int)
-- Every second it adds one to count. Another script reads them: see 09_ObjectVariables.lua.
-- ============================================================================

local timer = 0.0

function onUpdate()
    timer = timer + Brakeza:getDeltaTime()
    if timer >= 1.0 then
        timer = timer - 1.0
        count = count + 1
    end
end
