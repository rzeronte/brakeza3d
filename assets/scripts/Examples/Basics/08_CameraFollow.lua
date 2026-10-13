-- ============================================================================
-- Tutorial 08: A Camera that Follows an Object
-- Type: Global Script
-- ============================================================================
-- Make the camera follow an object smoothly, and decide who moves the camera.
--
-- Every frame the camera moves a small step (cameraSpeed) towards a point behind the followed object:
--   camera:setPosition(lerp(current, wanted, cameraSpeed))
--
-- The engine moves the camera with the keyboard by default; a follow camera must switch that off:
--   Components:Input():setKeyboardEnabled(false)   -- keyboard no longer moves the camera
--   Components:Input():setMouseEnabled(true)       -- the mouse is still read
--
-- Variables: nameObjectToFollow, cameraSpeed (0..1: higher = stiffer), cameraOffsetZ / cameraOffsetY (distance and height).
-- The followed object here is moved by the Movement lesson: W A S D, Space / C, I / K, Q / E.
-- ============================================================================

local followed = nil
local camera = nil
local forward = Vertex3D.new(0, 0, 1)

local function lerp(a, b, t)
    return a * (1 - t) + b * t
end

local function wantedPosition()
    local p = followed:getPosition()
    local f = forward:getScaled(cameraOffsetZ)
    return Vertex3D.new(p.x + f.x, p.y + cameraOffsetY, p.z + f.z)
end

function onStart()
    Components:Camera():setEnabled(true)
    Components:Input():setEnabled(true)
    Components:Input():setMouseEnabled(true)
    Components:Input():setKeyboardEnabled(false)   -- the keys now belong to the followed object

    camera = Components:Camera():getCamera()
    followed = Brakeza:getObjectByName(nameObjectToFollow)
    if followed ~= nil then
        camera:setPosition(wantedPosition())
        camera:LookAt(followed)
    end
end

function onUpdate()
    -- The followed object may load after this script: keep looking for it
    if followed == nil then
        followed = Brakeza:getObjectByName(nameObjectToFollow)
        return
    end

    local wanted = wantedPosition()
    local current = camera:getPosition()
    camera:setPosition(Vertex3D.new(
        lerp(current.x, wanted.x, cameraSpeed),
        lerp(current.y, wanted.y, cameraSpeed),
        lerp(current.z, wanted.z, cameraSpeed)))
    camera:LookAt(followed)
end

function onEnd()
    -- give the keyboard back to the camera
    Components:Input():setKeyboardEnabled(true)
end
