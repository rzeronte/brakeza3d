function onStart()
    print("Moving object: "..objectToMove)
    object = brakeza:getSceneObjectByLabel(objectToMove)
    isCollisionsEnabled = object:isCollisionsEnabled()
    if (isCollisionsEnabled) then
        print("[StartPositionObject] Collisionable Object moved!");
        object:disableSimulationCollider();
        object:setPosition(this:getPosition())
        object:UpdateShapeCollider();
        object:enableSimulationCollider()
    else
        print("[StartPositionObject] Object moved!");
        object:setPosition(this:getPosition())
    end

    local camera = componentsManager:getComponentCamera():getCamera()
    camera:setPosition(this:getPosition() + positionOffset)
end

function onUpdate()

    isCollisionsEnabled = this:isCollisionsEnabled()
end

function onCollision(with)
end
