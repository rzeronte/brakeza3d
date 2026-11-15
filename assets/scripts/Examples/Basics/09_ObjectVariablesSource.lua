--[[
 Brakeza3D - 3D/2D game engine
 Author:  Eduardo Rodríguez Álvarez
 GitHub:  https://github.com/rzeronte/brakeza3d
 Web:     https://brakeza.com
 Started: 2018-09-30
 Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 License: GNU General Public License v3.0 (see LICENSE)
--]]
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
