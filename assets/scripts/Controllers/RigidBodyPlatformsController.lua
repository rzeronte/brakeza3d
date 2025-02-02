function onStart()
    floor = brakeza:getSceneObjectByLabel("floor")
    isFloor = false
end

function onUpdate()
    local input = componentsManager:getComponentInput();
    local render = componentsManager:getComponentRender();
    local collisions = componentsManager:getComponentCollisions();

    from = this:getPosition()
    to = this:getPosition() + this:AxisUp():getScaled(2)
    render:drawLine(from, to, Color.new(0, 1, 0, 1))

    isFloor = collisions:isRayCollisionWith(from, to, floor)

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