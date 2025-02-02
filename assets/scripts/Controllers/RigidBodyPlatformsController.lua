function onStart()
    floor = brakeza:getSceneObjectByLabel("floor")
end

function onUpdate()
    local input = componentsManager:getComponentInput();

    isFloor = componentsManager:getComponentCollisions():isRayCollisionWith(
        this:getPosition(),
        this:getPosition() + this:AxisUp():getScaled(2),
        floor
    )

    local force = 25
    if input:isCharPressed("A") then
        this:applyCentralImpulse(Vertex3D.new(-force, 0, 0))
    end
    if input:isCharPressed("D") then
        this:applyCentralImpulse(Vertex3D.new(force, 0, 0))
    end
    if input:isCharPressed("SPACE") and input:isKeyEventDown() and isFloor then
        this:applyCentralImpulse(Vertex3D.new(0, -force * 50, 0))
    end
end

function onCollision(with)
end