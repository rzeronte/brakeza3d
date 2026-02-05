function onStart()
    object = Brakeza:getObjectByName(nameObjectToFollow)
    cameraOffset = Vertex3D.new(0.01, 0.01, 15)
    direction = Vertex3D.new(0, 0, 1)
    cameraSpeed = 0.0075
    local camera = Components:Camera():getCamera()
    camera:setPosition(object:getPosition() + direction:getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0));
    camera:LookAt(object)
end

function lerp(a, b, t)
    return a * (1 - t) + b * t
end

function onUpdate()

    local camera = Components:Camera():getCamera()
    local targetPosition = object:getPosition() + direction:getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0)
    local currentPosition = camera:getPosition()

    local newX = lerp(currentPosition.x, targetPosition.x, cameraSpeed)
    local newY = lerp(currentPosition.y, targetPosition.y, cameraSpeed)
    local newZ = lerp(currentPosition.z, targetPosition.z, cameraSpeed)

    camera:setPosition(Vertex3D.new(newX, newY, newZ))
    --camera:LookAt(object)
end

