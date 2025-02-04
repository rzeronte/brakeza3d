function onStart()
    floor = brakeza:getSceneObjectByLabel("floor")
    isFloor = false
end

function onUpdate()
    local input = componentsManager:getComponentInput();
    local render = componentsManager:getComponentRender();
    local collisions = componentsManager:getComponentCollisions();

    from = this:getPosition()
    local lengthRay = 1.5;
    toDown = this:getPosition() + this:AxisUp():getScaled(lengthRay)
    toDownLeft = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.5, 0, 0)
    toDownRight = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.5, 0, 0)

    render:drawLine(from, toDown, Color.new(1, 0, 0, 1))
    render:drawLine(from, toDownLeft, Color.new(0, 1, 0, 1))
    render:drawLine(from, toDownRight, Color.new(0, 0, 1, 1))

    isFloorDown = collisions:isRayCollisionWith(from, toDown, floor)
    isFloorDownLeft = collisions:isRayCollisionWith(from, toDownLeft, floor)
    isFloorDownRight = collisions:isRayCollisionWith(from, toDownRight, floor)

    isFloor = isFloorDown or isFloorDownLeft or isFloorDownRight

    if input:isCharPressed("A") and isFloor then
        this:applyCentralImpulse(Vertex3D.new(-speed, 0, 0))
    end

    if input:isCharPressed("D") and isFloor then
        this:applyCentralImpulse(Vertex3D.new(speed, 0, 0))
    end

    if input:isCharPressed("SPACE") and isFloor then
        this:applyCentralImpulse(Vertex3D.new(0, -speed * 10 , 0))
    end
end

function onCollision(with)
end