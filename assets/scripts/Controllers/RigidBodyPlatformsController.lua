function onStart()
    floor = brakeza:getSceneObjectByLabel("floor")
    isFloor = false
end

function onUpdate()
    local render = componentsManager:getComponentRender();
    local input = componentsManager:getComponentInput();
    local collisions = componentsManager:getComponentCollisions();
    local camera = componentsManager:getComponentCamera():getCamera()

    local newCamPos = this:getPosition() - this:AxisForward():getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0)
    camera:setPosition(newCamPos)
    camera:lookAt(this)

    -- Collision points
    local from = this:getPosition()
    local toDown = this:getPosition() - this:AxisUp():getScaled(lengthRay)
    local toUp = this:getPosition() + this:AxisUp():getScaled(lengthRay)

    local toDownLeft = this:getPosition() - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.5, 0, 0)
    local toDownRight = this:getPosition() - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.5, 0, 0)

    local toUpLeft = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.6, 0, 0);
    local toUpRight = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.6, 0, 0);

    -- Collision flags
    local isFloorDown = collisions:isRayCollisionWith(from, toDown, floor)
    local isFloorDownLeft = collisions:isRayCollisionWith(from, toDownLeft, floor)
    local isFloorDownRight = collisions:isRayCollisionWith(from, toDownRight, floor)
    local isFloor = isFloorDown or isFloorDownLeft or isFloorDownRight
    local isUp = collisions:isRayCollisionWith(from, toUp, floor)
    local isHookedLeft = collisions:isRayCollisionWith(from, toUpLeft, floor)
    local isHookedRight = collisions:isRayCollisionWith(from, toUpRight, floor)
    local isHooked = isHookedLeft or isHookedRight

    handleFloorMovement(input, isFloor)
    handleHook(input, isFloor, isHooked, isHookedLeft, isHookedRight, isUp)
    debug(input, render, isFloor, isHooked, isHookedLeft, isHookedRight, isUp, from, toDown, toDownLeft, toDownRight, toUp, toUpLeft, toUpRight)
end

function handleFloorMovement(input, isFloor)
    local dt = brakeza:getDeltaTime() -- Obtiene el deltaTime

    if isFloor then
        if input:isCharPressed("A") then
            --this:setRotation(M3:getMatrixRotationForEulerAngles(0, 0, 0))
            this:applyCentralForce(Vertex3D.new(-speed * dt, 0, 0))
        end

        if input:isCharPressed("D") then
            --this:setRotation(M3:getMatrixRotationForEulerAngles(0, 0, 0))
            this:applyCentralForce(Vertex3D.new(speed * dt, 0, 0))
        end

        if input:isCharFirstEventDown("SPACE") then
            local velocity = this:getLinearVelocity()
            local jumpVector = Vertex3D.new(velocity.x, jumpForce, velocity.z)
            this:applyCentralForce(jumpVector)
            print("Jump!")
        end
    else
        if input:isCharPressed("A") then
            this:applyCentralForce(Vertex3D.new(-speed * airControlFactor * dt, 0, 0))
            --this:setRotation(M3:getMatrixRotationForEulerAngles(180, 0, 0))
        end

        if input:isCharPressed("D") then
            this:applyCentralForce(Vertex3D.new(speed * airControlFactor * dt, 0, 0))
            --this:setRotation(M3:getMatrixRotationForEulerAngles(-180, 0, 0))
        end
    end
end

function handleHook(input, isFloor, isHooked, isHookedLeft, isHookedRight, isUp)
    local dt = brakeza:getDeltaTime() -- Obtiene el deltaTime

    if isHooked and not isFloor then
        if not (isHookedRight and isHookedLeft) and not isUp then
            if isHookedLeft and input:isCharPressed("D") then
                this:setLinearVelocity(Vertex3D.new(0, 0, 0))
            end
            if isHookedRight and input:isCharPressed("A") then
                this:setLinearVelocity(Vertex3D.new(0, 0, 0))
            end

            if input:isCharFirstEventDown("SPACE") then
                if isHookedLeft then
                    local jumpVector = Vertex3D.new(-speed * 10 * dt, -jumpForce, 0)
                    this:applyCentralForce(jumpVector)
                    print("Jump hook!")
                end
                if isHookedRight then
                    local jumpVector = Vertex3D.new(speed * 10 * dt, -jumpForce, 0)
                    this:applyCentralForce(jumpVector)
                    print("Jump hook!")
                end
            end
        end
    end
end

function debug(input, render, isFloor, isHooked, isHookedLeft, isHookedRight, isUp, from, toDown, toDownLeft, toDownRight, toUp, toUpLeft, toUpRight)
    local lineColorFloor = Color.new(1, 0, 0, 1)
    local lineColorUp = Color.new(1, 0, 0, 1)
    local lineColorHookedLeft = Color.new(1, 0, 0, 1)
    local lineColorHookedRight = Color.new(1, 0, 0, 1)

    if isFloor then
        lineColorFloor = Color.new(0, 1, 0, 1)
    else
        lineColorFloor = Color.new(1, 0, 0, 1)
    end

    if isHooked and not isFloor then
        if isHookedLeft and not isUp then
            lineColorHookedLeft = Color.new(0, 1, 0, 1)
        end
        if isHookedRight and not isUp then
            lineColorHookedRight = Color.new(0, 1, 0, 1)
        end

        if not (isHookedRight and isHookedLeft) and not isUp then
            if isHookedLeft and input:isCharPressed("D") then
                lineColorHookedLeft = Color.new(1, 1, 0, 1)
            end
            if isHookedRight and input:isCharPressed("A") then
                lineColorHookedRight = Color.new(1, 1, 0, 1)
            end
        end
    end

    if isUp then
        lineColorUp = Color.new(0, 1, 0, 1)
    end

    render:drawLine(from, toDown, lineColorFloor)
    render:drawLine(from, toDownLeft, lineColorFloor)
    render:drawLine(from, toDownRight, lineColorFloor)
    render:drawLine(from, toUp, Color.new(1, 1, 1, 1))
    render:drawLine(from, toUpLeft, lineColorHookedLeft)
    render:drawLine(from, toUpRight, lineColorHookedRight)
end

function onCollision(with)
end
