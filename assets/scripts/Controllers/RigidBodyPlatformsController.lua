function onStart()
    floor = brakeza:getSceneObjectByLabel("floor")
    --this:disableDeactivationCollider()

    isFloor = false
    isHooked = false

    isUp = false
    isHookedLeft = false
    isHookedRight = false
    isFloorDown = false
    isFloorDownLeft = false
    isFloorDownRight = false

    isBorderUpLeftVertical = false
    isBorderUpRightVertical = false

    isBorderUpLeftHorizontal = false
    isBorderUpRightHorizontal = false

    isBorderBottomLeftVertical = false
    isBorderBottomRightVertical = false

    isCornerUpLeft = false
    isCornerUpRight = false

    isFullFreeCornerUpLeft = false
    isFullFreeCornerUpRight = false;
    isFullFreeCornerBottomLeft = false
    isFullFreeCornerBottomRight = false

    isBlocked = false
    countUnblockableFrames = 0
end

function onUpdate()
    local render = componentsManager:getComponentRender();
    local input = componentsManager:getComponentInput();
    local collisions = componentsManager:getComponentCollisions();
    local camera = componentsManager:getComponentCamera():getCamera()

    local newCamPos = this:getPosition() + this:AxisForward():getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0)
    camera:setPosition(newCamPos)
    camera:lookAt(this)

    -- Collision points
    local from = this:getPosition()
    local toDown = this:getPosition() - this:AxisUp():getScaled(lengthRay)
    local toUp = this:getPosition() + this:AxisUp():getScaled(lengthRay)

    local toDownLeft = this:getPosition() - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.5, 0, 0)
    local toDownRight = this:getPosition() - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.5, 0, 0)

    local toUpLeft = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.6, 0, 0)
    local toUpRight = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.6, 0, 0)

    local toBottomLeft = this:getPosition() + this:AxisUp():getScaled(lengthRay + 1.5 ) + Vertex3D.new(0.6, 0, 0)
    local toBottomRight = this:getPosition() + this:AxisUp():getScaled(lengthRay + 1.5) + Vertex3D.new(-0.6, 0, 0)

    -- corner centers
    local cornerUpLeftCenter = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.6, 0, 0)
    local cornerUpRightCenter = this:getPosition() + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.6, 0, 0)

    local cornerBottomLeftCenter = this:getPosition() + this:AxisUp():getScaled(lengthRay + 1 ) + Vertex3D.new(0.6, 0, 0)
    local cornerBottomRightCenter = this:getPosition() + this:AxisUp():getScaled(lengthRay + 1) + Vertex3D.new(-0.6, 0, 0)

    -- border up
    local fromBorderUpLeftVertical, toBorderUpLeftVertical, fromBorderUpLeftHorizontal, toBorderUpLeftHorizontal = getCrossBorders(cornerUpLeftCenter)
    local fromBorderUpRightHorizontal, toBorderUpRightHorizontal, fromBorderUpRightVertical, toBorderUpRightVertical = getCrossBorders(cornerUpRightCenter)

    -- border bottom
    local fromBorderBottomLeftVertical, toBorderBottomLeftVertical, fromBorderBottomLeftHorizontal, toBorderBottomLeftHorizontal = getCrossBorders(cornerBottomLeftCenter)
    local fromBorderBottomRightHorizontal, toBorderBottomRightHorizontal, fromBorderBottomRightVertical, toBorderBottomRightVertical = getCrossBorders(cornerBottomRightCenter)
    
    -- Collision flags
    isFloorDown = collisions:isRayCollisionWith(from, toDown, floor)
    isFloorDownLeft = collisions:isRayCollisionWith(from, toDownLeft, floor)
    isFloorDownRight = collisions:isRayCollisionWith(from, toDownRight, floor)
    isFloor = isFloorDown and isFloorDownLeft and isFloorDownRight

    isUp = collisions:isRayCollisionWith(from, toUp, floor)
    isHookedLeft = collisions:isRayCollisionWith(from, toUpLeft, floor)
    isHookedRight = collisions:isRayCollisionWith(from, toUpRight, floor)
    isHooked = isHookedLeft or isHookedRight

    -- corners up
    isBorderUpLeftVertical = collisions:isRayCollisionWith(fromBorderUpLeftVertical, toBorderUpLeftVertical, floor)
    isBorderUpLeftHorizontal = collisions:isRayCollisionWith(fromBorderUpLeftHorizontal, toBorderUpLeftHorizontal, floor)
    isCornerUpLeft = isBorderUpLeftVertical and isBorderUpLeftHorizontal
    isFullFreeCornerUpLeft = not isBorderUpLeftVertical and not isBorderUpLeftHorizontal

    isBorderUpRightVertical = collisions:isRayCollisionWith(fromBorderUpRightVertical, toBorderUpRightVertical, floor)
    isBorderUpRightHorizontal = collisions:isRayCollisionWith(fromBorderUpRightHorizontal, toBorderUpRightHorizontal, floor)
    isCornerUpRight = isBorderUpRightVertical and isBorderUpRightHorizontal
    isFullFreeCornerUpRight = not isBorderUpRightVertical and not isBorderUpRightHorizontal

    -- corners bottom
    isBorderBottomLeftVertical = collisions:isRayCollisionWith(fromBorderBottomLeftVertical, toBorderBottomLeftVertical, floor)
    isBorderBottomLeftHorizontal = collisions:isRayCollisionWith(fromBorderBottomLeftHorizontal, toBorderBottomLeftHorizontal, floor)
    isCornerBottomLeft = isBorderBottomLeftVertical and isBorderBottomLeftHorizontal
    isFullFreeCornerBottomLeft = not isBorderBottomLeftVertical and not isBorderBottomLeftHorizontal

    isBorderBottomRightVertical = collisions:isRayCollisionWith(fromBorderBottomRightVertical, toBorderBottomRightVertical, floor)
    isBorderBottomRightHorizontal = collisions:isRayCollisionWith(fromBorderBottomRightHorizontal, toBorderBottomRightHorizontal, floor)
    isCornerBottomRight = isBorderBottomRightVertical and isBorderBottomRightHorizontal
    isFullFreeCornerBottomRight = not isBorderBottomRightVertical and not isBorderBottomRightHorizontal

    -- decrease frame block counter
    countUnblockableFrames = countUnblockableFrames - 1
    if countUnblockableFrames < 0 then
        countUnblockableFrames = 0
    end

    handleFloorMovement(input)
    handleHook(input)
    debug(
        input,
        render,
        from,
        toDown,
        toDownLeft,
        toDownRight,
        toUp,
        toUpLeft,
        toUpRight,
        fromBorderUpLeftVertical, toBorderUpLeftVertical, fromBorderUpRightVertical, toBorderUpRightVertical,
        fromBorderUpLeftHorizontal, toBorderUpLeftHorizontal, fromBorderUpRightHorizontal, toBorderUpRightHorizontal,
        fromBorderBottomLeftVertical, toBorderBottomLeftVertical, fromBorderBottomLeftHorizontal, toBorderBottomLeftHorizontal,
        fromBorderBottomRightHorizontal, toBorderBottomRightHorizontal, fromBorderBottomRightVertical, toBorderBottomRightVertical
   )
end

function handleFloorMovement(input)
    if isBlocked then
        print("Cancel movement by isBlocked")
        return
    end

    if isHooked and not isFloor then
        return
    end

    local dt = brakeza:getDeltaTime() -- Obtiene deltaTime
    local velocity = this:getLinearVelocity() -- Obtiene la velocidad actual

    if isFloor then
        if input:isCharPressed("A") then
            this:enableSimulationCollider()
            this:applyCentralForce(Vertex3D.new(-speed * dt * 1000, 0, 0))
            -- this:setRotation(M3:getMatrixRotationForEulerAngles(180, 0, 0))
        end

        if input:isCharPressed("D") then
            this:enableSimulationCollider()
            this:applyCentralForce(Vertex3D.new(speed * dt * 1000, 0, 0))
            -- this:setRotation(M3:getMatrixRotationForEulerAngles(0, 0, 0))
        end

        if math.abs(velocity.x) > maxSpeed then
            velocity.x = maxSpeed * (velocity.x > 0 and 1 or -1)
            this:setLinearVelocity(velocity)
        end
        if input:isCharFirstEventDown("SPACE") and isFloor then
            local jumpVector = Vertex3D.new(0, jumpForce, 0)
            this:applyCentralImpulse(jumpVector)
            print("Jump!")
        end

    else
        if input:isCharPressed("A") then
            this:applyCentralForce(Vertex3D.new(-speed * airControlFactor * dt * 1000, 0, 0))
        end

        if input:isCharPressed("D") then
            this:applyCentralForce(Vertex3D.new(speed * airControlFactor * dt * 1000, 0, 0))
        end

        -- Reducir la velocidad en el aire si es muy alta
        local maxAirSpeed = 3.0
        if math.abs(velocity.x) > maxAirSpeed then
            velocity.x = maxAirSpeed * (velocity.x > 0 and 1 or -1)
            this:setLinearVelocity(velocity)
        end
    end
end

function handleHook(input)
    local dt = brakeza:getDeltaTime()
    local v = this:getLinearVelocity()

    if isBlocked then
        if input:isCharFirstEventDown("SPACE") then
            print("Freedom!")
            this:setLinearVelocity(Vertex3D.new(0, -1, 0))
            this:enableSimulationCollider()
            countUnblockableFrames = 50
            isBlocked = false
        end
        return
    else
        if isCornerUpLeft and isFullFreeCornerBottomLeft and countUnblockableFrames == 0 and not isFloor and isFullFreeCornerUpRight and v.y < 0 then
            print("Cross Up Left!")
            this:disableSimulationCollider()
            isBlocked = true
            return
        end

        if isCornerUpRight and isFullFreeCornerBottomRight and  countUnblockableFrames == 0 and not isFloor and isFullFreeCornerUpLeft and v.y < 0 then
            print("Cross Up Right!")
            this:disableSimulationCollider()
            isBlocked = true
            return
        end
    end

    if (isHookedRight and isHookedLeft and isUp) then
        print("Simultaneos UP colliders, returning!")
        return
    end

    if isHookedLeft and input:isCharPressed("D") then
        print("stop r")
        local v = this:getLinearVelocity();
        v.y = v.y * 0.95;
        this:setLinearVelocity(v);
        return
    end
    if isHookedRight and input:isCharPressed("A") then
        print("stop l")
        local v = this:getLinearVelocity();
        v.y = v.y * 0.95;
        this:setLinearVelocity(v);
        return
    end
    if isHookedLeft then
        if input:isCharFirstEventDown("SPACE") and not isFloor then
            this:enableSimulationCollider()
            local jumpVector = Vertex3D.new(-100, jumpForce, 0)
            this:applyCentralImpulse(jumpVector)
            print("Jump hook l!")
        end
    end
    if isHookedRight then
        if input:isCharFirstEventDown("SPACE") and not isFloor then
            this:sleepCollider()
            local jumpVector = Vertex3D.new(100, jumpForce, 0)
            this:applyCentralImpulse(jumpVector)
            print("Jump hook r!")
        end
    end
end

function debug(
    input,
    render,
    from,
    toDown,
    toDownLeft,
    toDownRight,
    toUp,
    toUpLeft,
    toUpRight,
    fromBorderUpLeftVertical, toBorderUpLeftVertical, fromBorderUpRightVertical, toBorderUpRightVertical,
    fromBorderUpLeftHorizontal, toBorderUpLeftHorizontal, fromBorderUpRightHorizontal, toBorderUpRightHorizontal,
    fromBorderBottomLeftVertical, toBorderBottomLeftVertical, fromBorderBottomLeftHorizontal, toBorderBottomLeftHorizontal,
    fromBorderBottomRightHorizontal, toBorderBottomRightHorizontal, fromBorderBottomRightVertical, toBorderBottomRightVertical
 )
    local lineColorFloorLeft = Color.new(1, 0, 0, 1)
    local lineColorFloorRight = Color.new(1, 0, 0, 1)
    local lineColorFloorDown = Color.new(1, 0, 0, 1)

    local lineColorUp = Color.new(1, 0, 0, 1)
    local lineColorHookedLeft = Color.new(1, 0, 0, 1)
    local lineColorHookedRight = Color.new(1, 0, 0, 1)

    -- corner up left
    local lineColorBorderUpLeftVertical = Color.new(1, 0, 0, 1)
    local lineColorBorderUpLeftHorizontal = Color.new(1, 0, 0, 1)

    -- corner up right
    local lineColorBorderUpRightVertical = Color.new(1, 0, 0, 1)
    local lineColorBorderUpRightHorizontal = Color.new(1, 0, 0, 1)

    -- corner bottom left
    local lineColorBorderBottomLeftVertical = Color.new(1, 0, 0, 1)
    local lineColorBorderBottomLeftHorizontal = Color.new(1, 0, 0, 1)

    -- corner bottom right
    local lineColorBorderBottomRightVertical = Color.new(1, 0, 0, 1)
    local lineColorBorderBottomRightHorizontal = Color.new(1, 0, 0, 1)

    if isFloorDown then
        lineColorFloorDown = Color.new(0, 1, 0, 1)
    end

    if isFloorDownLeft then
        lineColorFloorLeft = Color.new(0, 1, 0, 1)
    end

    if isFloorDownRight then
        lineColorFloorRight = Color.new(0, 1, 0, 1)
    end

    if isHookedLeft then
        lineColorHookedLeft = Color.new(0, 1, 0, 1)
    end
    if isHookedRight then
        lineColorHookedRight = Color.new(0, 1, 0, 1)
    end

    if isUp then
        lineColorUp = Color.new(0, 1, 0, 1)
    end
    -- up
    if isBorderUpLeftVertical then
        lineColorBorderUpLeftVertical = Color.new(0, 1, 0, 1)
    end

    if isBorderUpRightVertical then
        lineColorBorderUpRightVertical = Color.new(0, 1, 0, 1)
    end

    if isBorderUpLeftHorizontal then
        lineColorBorderUpLeftHorizontal = Color.new(0, 1, 0, 1)
    end

    if isBorderUpRightHorizontal then
        lineColorBorderUpRightHorizontal = Color.new(0, 1, 0, 1)
    end

    -- bottom
    if isBorderBottomLeftVertical then
        lineColorBorderBottomLeftVertical = Color.new(0, 1, 0, 1)
    end

    if isBorderBottomRightVertical then
        lineColorBorderBottomRightVertical = Color.new(0, 1, 0, 1)
    end

    if isBorderBottomLeftHorizontal then
        lineColorBorderBottomLeftHorizontal = Color.new(0, 1, 0, 1)
    end

    if isBorderBottomRightHorizontal then
        lineColorBorderBottomRightHorizontal = Color.new(0, 1, 0, 1)
    end

    render:drawLine(from, toDown, lineColorFloorDown)

    render:drawLine(from, toDownLeft, lineColorFloorLeft)
    render:drawLine(from, toDownRight, lineColorFloorRight)

    render:drawLine(from, toUp, lineColorUp)

    render:drawLine(from, toUpLeft, lineColorHookedLeft)
    render:drawLine(from, toUpRight, lineColorHookedRight)

    render:drawLine(fromBorderUpLeftVertical, toBorderUpLeftVertical, lineColorBorderUpLeftVertical)
    render:drawLine(fromBorderUpRightVertical, toBorderUpRightVertical, lineColorBorderUpRightVertical)
    render:drawLine(fromBorderUpLeftHorizontal, toBorderUpLeftHorizontal, lineColorBorderUpLeftHorizontal)
    render:drawLine(fromBorderUpRightHorizontal, toBorderUpRightHorizontal, lineColorBorderUpRightHorizontal)

    render:drawLine(fromBorderBottomLeftVertical, toBorderBottomLeftVertical, lineColorBorderBottomLeftVertical)
    render:drawLine(fromBorderBottomLeftHorizontal, toBorderBottomLeftHorizontal, lineColorBorderBottomLeftHorizontal)
    render:drawLine(fromBorderBottomRightHorizontal, toBorderBottomRightHorizontal, lineColorBorderBottomRightHorizontal)
    render:drawLine(fromBorderBottomRightVertical, toBorderBottomRightVertical, lineColorBorderBottomRightVertical)
end

function onCollision(with)
end

function getCrossBorders(center)
    local size = 0.5

    -- Calcular los valores basados en size
    local halfSize = size * 0.5  -- Mitad del tamaño total
    local quarterSize = size * 0.25  -- Un cuarto del tamaño total

    -- Calcular los puntos verticales (línea de arriba hacia abajo)
    local fromBorderVertical = center - this:AxisUp():getScaled(halfSize)
    local toBorderVertical = center + this:AxisUp():getScaled(halfSize)

    -- Calcular los puntos horizontales (línea de izquierda a derecha)
    local fromBorderHorizontal = center - this:AxisRight():getScaled(halfSize)
    local toBorderHorizontal = center + this:AxisRight():getScaled(halfSize)

    -- Retornar las cuatro variables individualmente
    return fromBorderVertical, toBorderVertical, fromBorderHorizontal, toBorderHorizontal
end