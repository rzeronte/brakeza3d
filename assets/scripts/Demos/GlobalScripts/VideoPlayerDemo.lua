local overlay = nil
local target  = nil
local done    = false

function onStart()
    target = Brakeza:getObjectByName(objectToMove)

    overlay = ObjectFactory.Image2D("", 0, 0, 1.0, 1.0)
    overlay:setScreenPosition(0, 0)
    overlay:loadVideo("../assets/videos/video.mp4")
end

function onUpdate()
    if done then return end

    if overlay ~= nil and overlay:isVideoFinished() then
        overlay:stopVideo()
        Brakeza:removeObject(overlay)
        overlay = nil
        done = true

        if target ~= nil then
            target:setPosition(target:getPosition() + positionOffset)
        end
    end
end
