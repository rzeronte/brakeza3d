function onStart()
    floor = brakeza:getSceneObjectByLabel("floor")
    isFloor = false
end

function limitVelocity(max)
    local velocity = this:getLinearVelocity()

    print("Module velocity..."..velocity:getModule())

    if velocity:getModule() > max then
        this:setLinearVelocity(velocity:getNormalize():getScaled(max))
    end
end

function onUpdate()
    local input = componentsManager:getComponentInput();
    local render = componentsManager:getComponentRender();
    local collisions = componentsManager:getComponentCollisions();
    local camera = componentsManager:getComponentCamera():getCamera()

    local forward = this:AxisForward()
    local newCamPos = this:getPosition() + forward:getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0)
    camera:setPosition(newCamPos)
    camera:lookAt(this)

    -- Collision points
    local from = this:getPosition()
    local toDown = this:getPosition() + this:AxisUp():getScaled(lengthRay)

    local toDownLeft = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.5, 0, 0)
    local toDownRight = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.5, 0, 0)

    local toUpLeft = this:getPosition() - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.6, 0, 0);
    local toUpRight = this:getPosition() - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.6, 0, 0);

    -- Collision flags
    local isFloorDown = collisions:isRayCollisionWith(from, toDown, floor)
    local isFloorDownLeft = collisions:isRayCollisionWith(from, toDownLeft, floor)
    local isFloorDownRight = collisions:isRayCollisionWith(from, toDownRight, floor)
    local isFloor = isFloorDown or isFloorDownLeft or isFloorDownRight
    local lineColorFloor = Color.new(1, 0, 0, 1)

    local isHookedLeft = collisions:isRayCollisionWith(from, toUpLeft, floor)
    local isHookedRight = collisions:isRayCollisionWith(from, toUpRight, floor)
    local isHooked = isHookedLeft or isHookedRight
    local lineColorHooked = Color.new(1, 0, 0, 1)

    if isFloor then
        if input:isCharPressed("A") then
            this:applyCentralForce(Vertex3D.new(-speed, 0, 0))
        end

        if input:isCharPressed("D") then
            this:applyCentralForce(Vertex3D.new(speed, 0, 0))
        end

        if input:isCharFirstEventDown("SPACE") then
            local velocity = this:getLinearVelocity()
            local jumpVector = Vertex3D.new(velocity.x, -jumpForce, velocity.z)
            this:applyCentralForce(jumpVector)
            print("jump")
        end
        lineColorFloor = Color.new(0, 1, 0, 1)
    else
        if input:isCharPressed("A") then
            this:applyCentralForce(Vertex3D.new(-speed * airControlFactor, 0, 0))
        end

        if input:isCharPressed("D") then
            this:applyCentralForce(Vertex3D.new(speed * airControlFactor, 0, 0))
        end
        lineColorFloor = Color.new(1, 0, 0, 1)
    end

    if isHooked then
        lineColorHooked = Color.new(0, 1, 0, 1)
    end

    -- debug
    render:drawLine(from, toDown, lineColorFloor)
    render:drawLine(from, toDownLeft, lineColorFloor)
    render:drawLine(from, toDownRight, lineColorFloor)

    render:drawLine(from, toUpLeft, lineColorHooked)
    render:drawLine(from, toUpRight, lineColorHooked)

    local velocity = this:getLinearVelocity()
    --print("Module velocity: x: " .. velocity.x .. ", y: " .. velocity.y .. ", z:" .. velocity.z)
end

function onCollision(with)
end