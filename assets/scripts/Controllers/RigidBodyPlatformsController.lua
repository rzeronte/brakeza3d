local State = {
    IDLE = "idle",
    WALKING = "walking",
    JUMPING = "jumping",
    FALLING = "falling",
    HOOKED = "hooked",
    HANGLING = "hanging"
}

local OffsetCollideres = {
    IDLE = Vertex3D.new(0, 0, 0),
    WALKING = Vertex3D.new(0, 0, 0),
    JUMPING = Vertex3D.new(0, 0, 0),
    RUNNING = Vertex3D.new(0, 0, 0),
    FALLING = Vertex3D.new(0, 0, 0),
    HOOKED = Vertex3D.new(0, 0, 0),
    HANGLING = Vertex3D.new(0, 0, 0)
}

local function createRayPositions(pos, this, lengthRay)
    local rays = {
        toDown = pos - this:AxisUp():getScaled(lengthRay),
        toUp = pos + this:AxisUp():getScaled(lengthRay),

        toDownLeft = pos - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.4, 0, 0),
        toDownRight = pos - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.4, 0, 0),

        toUpLeft = pos + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.4, 0, 0),
        toUpRight = pos + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.4, 0, 0),

        toBottomLeft = pos + this:AxisUp():getScaled(lengthRay + 1.5) + Vertex3D.new(0.4, 0, 0),
        toBottomRight = pos + this:AxisUp():getScaled(lengthRay + 1.5) + Vertex3D.new(-0.4, 0, 0),

        cornerUpLeftCenter = pos + this:AxisUp():getScaled(1) + Vertex3D.new(0.3, 0, 0),
        cornerUpRightCenter = pos + this:AxisUp():getScaled(1) + Vertex3D.new(-0.3, 0, 0),

        cornerBottomLeftCenter = pos + this:AxisUp():getScaled(1 + 0.5) + Vertex3D.new(0.3, 0, 0),
        cornerBottomRightCenter = pos + this:AxisUp():getScaled(1 + 0.5) + Vertex3D.new(-0.3, 0, 0),
    }

    return rays
end

function getCrossBorders(center)
    local size = 0.4

    local halfSize = size * 0.4

    local fromBorderVertical = center - this:AxisUp():getScaled(halfSize)
    local toBorderVertical = center + this:AxisUp():getScaled(halfSize)

    local fromBorderHorizontal = center - this:AxisRight():getScaled(halfSize)
    local toBorderHorizontal = center + this:AxisRight():getScaled(halfSize)

    return fromBorderVertical, toBorderVertical, fromBorderHorizontal, toBorderHorizontal
end

function createBorderCross(cornerUpLeftCenter, cornerUpRightCenter, cornerBottomLeftCenter, cornerBottomRightCenter)
    -- border up
    local fromBorderUpLeftVertical, toBorderUpLeftVertical, fromBorderUpLeftHorizontal, toBorderUpLeftHorizontal = getCrossBorders(cornerUpLeftCenter)
    local fromBorderUpRightHorizontal, toBorderUpRightHorizontal, fromBorderUpRightVertical, toBorderUpRightVertical = getCrossBorders(cornerUpRightCenter)

    -- border bottom
    local fromBorderBottomLeftVertical, toBorderBottomLeftVertical, fromBorderBottomLeftHorizontal, toBorderBottomLeftHorizontal = getCrossBorders(cornerBottomLeftCenter)
    local fromBorderBottomRightHorizontal, toBorderBottomRightHorizontal, fromBorderBottomRightVertical, toBorderBottomRightVertical = getCrossBorders(cornerBottomRightCenter)

    -- Bordes
    local borders = {
        upLeft = {
            fromBorderVertical = fromBorderUpLeftVertical,
            toBorderVertical = toBorderUpLeftVertical,
            fromBorderHorizontal = fromBorderUpLeftHorizontal,
            toBorderHorizontal = toBorderUpLeftHorizontal
        },
        upRight = {
            fromBorderVertical = fromBorderUpRightVertical,
            toBorderVertical = toBorderUpRightVertical,
            fromBorderHorizontal = fromBorderUpRightHorizontal,
            toBorderHorizontal = toBorderUpRightHorizontal
        },
        bottomLeft = {
            fromBorderVertical = fromBorderBottomLeftVertical,
            toBorderVertical = toBorderBottomLeftVertical,
            fromBorderHorizontal = fromBorderBottomLeftHorizontal,
            toBorderHorizontal = toBorderBottomLeftHorizontal
        },
        bottomRight = {
            fromBorderVertical = fromBorderBottomRightVertical,
            toBorderVertical = toBorderBottomRightVertical,
            fromBorderHorizontal = fromBorderBottomRightHorizontal,
            toBorderHorizontal = toBorderBottomRightHorizontal
        }
    }

    return borders
end

function UpdateCollisionFlags(from, rays, cross)
    local collisions = componentsManager:getComponentCollisions();
    local velocity = this:getLinearVelocity()

    -- Collision flags
    isFloorDown = collisions:isRayCollisionWith(from, rays.toDown, floor)
    isFloorDownLeft = collisions:isRayCollisionWith(from, rays.toDownLeft, floor)
    isFloorDownRight = collisions:isRayCollisionWith(from, rays.toDownRight, floor)
    isFloor = isFloorDown and (isFloorDownLeft or isFloorDownRight) and velocity.y <= 1

    isUp = collisions:isRayCollisionWith(from, rays.toUp, floor)
    isHookedLeft = collisions:isRayCollisionWith(from, rays.toUpLeft, floor)
    isHookedRight = collisions:isRayCollisionWith(from, rays.toUpRight, floor)
    isHooked = (isHookedLeft or isHookedRight) and not isFloorDown

    -- corners up
    isBorderUpLeftVertical = collisions:isRayCollisionWith(cross.upLeft.fromBorderVertical, cross.upLeft.toBorderVertical, floor)
    isBorderUpLeftHorizontal = collisions:isRayCollisionWith(cross.upLeft.fromBorderHorizontal, cross.upLeft.toBorderHorizontal, floor)
    isCornerUpLeft = isBorderUpLeftVertical and isBorderUpLeftHorizontal
    isFullFreeCornerUpLeft = not isBorderUpLeftVertical and not isBorderUpLeftHorizontal

    isBorderUpRightVertical = collisions:isRayCollisionWith(cross.upRight.fromBorderVertical, cross.upRight.toBorderVertical, floor)
    isBorderUpRightHorizontal = collisions:isRayCollisionWith(cross.upRight.fromBorderHorizontal, cross.upRight.toBorderHorizontal, floor)
    isCornerUpRight = isBorderUpRightVertical and isBorderUpRightHorizontal
    isFullFreeCornerUpRight = not isBorderUpRightVertical and not isBorderUpRightHorizontal

    -- corners bottom
    isBorderBottomLeftVertical = collisions:isRayCollisionWith(cross.bottomLeft.fromBorderVertical, cross.bottomLeft.toBorderVertical, floor)
    isBorderBottomLeftHorizontal = collisions:isRayCollisionWith(cross.bottomLeft.fromBorderHorizontal, cross.bottomLeft.toBorderHorizontal, floor)
    isCornerBottomLeft = isBorderBottomLeftVertical and isBorderBottomLeftHorizontal
    isFullFreeCornerBottomLeft = not isBorderBottomLeftVertical and not isBorderBottomLeftHorizontal

    isBorderBottomRightVertical = collisions:isRayCollisionWith(cross.bottomRight.fromBorderVertical, cross.bottomRight.toBorderVertical, floor)
    isBorderBottomRightHorizontal = collisions:isRayCollisionWith(cross.bottomRight.fromBorderHorizontal, cross.bottomRight.toBorderHorizontal, floor)
    isCornerBottomRight = isBorderBottomRightVertical and isBorderBottomRightHorizontal
    isFullFreeCornerBottomRight = not isBorderBottomRightVertical and not isBorderBottomRightHorizontal
end

function UpdateAnimationFromState()
    if (currentState == State.JUMPING) then
        this:setAnimationByName("Armature|POSE_JUMPING")
        this:setColliderOffset(OffsetCollideres.JUMPING)
    elseif (currentState == State.RUNNING) then
        this:setAnimationByName("Armature|RUNNING")
        this:setColliderOffset(OffsetCollideres.RUNNING)
    elseif (currentState == State.FALLING) then
        this:setAnimationByName("Armature|POSE_FALLING")
        this:setColliderOffset(OffsetCollideres.FALLING)
    elseif (currentState == State.IDLE) then
        this:setAnimationByName("Armature|IDLE")
        this:setColliderOffset(OffsetCollideres.IDLE)
    elseif (currentState == State.HANGLING) then
        this:setAnimationByName("Armature|HANGLING_IDLE")
        this:setColliderOffset(OffsetCollideres.HANGLING)
    elseif (currentState == State.WALKING) then
        this:setAnimationByName("Armature|WALKING")
        this:setColliderOffset(OffsetCollideres.WALKING)
    end
end

------------------------------------------------------------------------------------------------------------------------
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

    isBlockedByHook = false
    countUnblockableFrames = 0

    rotationLeft = M3:getMatrixRotationForEulerAngles(180, 0, 0);
    rotationRight = M3:getMatrixRotationForEulerAngles(0, 0, 0);
    sideRotation = rotationLeft;

    currentState = State.IDLE
end

function onUpdate()
    local input = componentsManager:getComponentInput();
    local camera = componentsManager:getComponentCamera():getCamera()

    camera:setPosition(this:getPosition() + this:AxisForward():getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0) - this:getColliderOffset())
    camera:lookAt(this)

    local pos = this:getPosition()
    currentState = currentState

    -- Collision points
    local rays = createRayPositions(pos, this, lengthRay)
    local cross = createBorderCross(rays.cornerUpLeftCenter, rays.cornerUpRightCenter, rays.cornerBottomLeftCenter, rays.cornerBottomRightCenter)

    UpdateAnimationFromState()
    UpdateCollisionFlags(pos, rays, cross)

    -- decrease frame block counter
    countUnblockableFrames = countUnblockableFrames - 1
    if countUnblockableFrames < 0 then
        countUnblockableFrames = 0
    end

    this:setRotation(sideRotation)
    local dt = brakeza:getDeltaTime()

    handleFloorMovement(input, dt)
    handleHook(input, dt)

    debug(pos, rays, cross)
end

function handleFloorMovement(input, dt)
    if isBlockedByHook then
        return
    end

    if isHooked and not isFloor then
        return
    end

    local velocity = this:getLinearVelocity() -- Obtiene la velocidad actual

    if isFloor then

        if input:isCharFirstEventDown("SPACE") then
            local jumpVector = Vertex3D.new(0, jumpForce, 0)
            this:applyCentralImpulse(jumpVector)
            currentState = State.JUMPING
            print("Jump!")
            return
        end

        if input:isCharPressed("A") then
            print("moving left")
            currentState = State.RUNNING
            this:enableSimulationCollider()
            this:applyCentralForce(Vertex3D.new(-speed * dt * 1000, 0, 0))
            sideRotation = rotationLeft
        end

        if input:isCharPressed("D") then
            print("moving right")
            currentState = State.RUNNING
            this:enableSimulationCollider()
            this:applyCentralForce(Vertex3D.new(speed * dt * 1000, 0, 0))
            sideRotation = rotationRight
        end

        if math.abs(velocity.x) > maxSpeed then
            velocity.x = maxSpeed * (velocity.x > 0 and 1 or -1)
            this:setLinearVelocity(velocity)
        end

        if math.abs(velocity.x) <= 2.25 then
                currentState = State.WALKING
                if math.abs(velocity.x) <= 0.1 then
                    currentState = State.IDLE
                end
            else
                currentState = State.RUNNING
        end
    else
        if input:isCharPressed("A") then
            sideRotation = rotationLeft
            this:applyCentralForce(Vertex3D.new(-speed * airControlFactor * dt * 1000, 0, 0))
        end

        if input:isCharPressed("D") then
            sideRotation = rotationRight
            this:applyCentralForce(Vertex3D.new(speed * airControlFactor * dt * 1000, 0, 0))
        end

        -- Reducir la velocidad en el aire si es muy alta
        local maxAirSpeed = 3.0
        if math.abs(velocity.x) > maxAirSpeed then
            velocity.x = maxAirSpeed * (velocity.x > 0 and 1 or -1)
            this:setLinearVelocity(velocity)
        end

        if velocity.y < 0 then
            currentState = State.FALLING
        end
    end
end

function handleHook(input, dt)
    local v = this:getLinearVelocity()

    if isBlockedByHook then
        if input:isCharFirstEventDown("SPACE") then
            print("Freedom!")
            this:setLinearVelocity(Vertex3D.new(0, -1, 0))
            this:enableSimulationCollider()
            countUnblockableFrames = 50
            isBlockedByHook = false
        return
        end
    else
        if isCornerUpRight and isFullFreeCornerBottomRight and  countUnblockableFrames == 0 and not isFloor and isFullFreeCornerUpLeft and v.y < 0 then
            print("Cross Up Right!")
            currentState = State.HANGLING
            this:disableSimulationCollider()
            isBlockedByHook = true
            sideRotation = rotationLeft
            return
        end

        if isCornerUpLeft and isFullFreeCornerBottomLeft and countUnblockableFrames == 0 and not isFloor and isFullFreeCornerUpRight and v.y < 0 then
            print("Cross Up Left!")
            currentState = State.HANGLING
            this:disableSimulationCollider()
            isBlockedByHook = true
            sideRotation = rotationRight
            return
        end
    end

    if (isHookedRight and isHookedLeft and isUp) then
        return
    end

    if isHookedLeft and not isFloorDown and input:isCharPressed("D") then
        print("stop r")
        local v = this:getLinearVelocity();
        v.y = v.y * 0.95;
        this:setLinearVelocity(v);
        return
    end
    if isHookedRight and not isFloorDown and input:isCharPressed("A") then
        print("stop l")
        local v = this:getLinearVelocity();
        v.y = v.y * 0.95;
        this:setLinearVelocity(v);
        return
    end
    if isHookedLeft and not isFloorDown then
        if input:isCharFirstEventDown("SPACE") and not isFloor then
            this:enableSimulationCollider()
            local jumpVector = Vertex3D.new(-100, jumpForce, 0)
            this:applyCentralImpulse(jumpVector)
            print("Jump hook l!")
        end
    end
    if isHookedRight and not isFloorDown then
        if input:isCharFirstEventDown("SPACE") and not isFloor then
            this:sleepCollider()
            local jumpVector = Vertex3D.new(100, jumpForce, 0)
            this:applyCentralImpulse(jumpVector)
            print("Jump hook r!")
        end
    end
end

function debug(from, rays, cross)
    local render = componentsManager:getComponentRender()

    local colors = {
        floorLeft = Color.new(1, 0, 0, 1),
        floorRight = Color.new(1, 0, 0, 1),
        floorDown = Color.new(1, 0, 0, 1),
        up = Color.new(1, 0, 0, 1),
        hookedLeft = Color.new(1, 0, 0, 1),
        hookedRight = Color.new(1, 0, 0, 1),
        borderUpLeftVertical = Color.new(1, 0, 0, 1),
        borderUpLeftHorizontal = Color.new(1, 0, 0, 1),
        borderUpRightVertical = Color.new(1, 0, 0, 1),
        borderUpRightHorizontal = Color.new(1, 0, 0, 1),
        borderBottomLeftVertical = Color.new(1, 0, 0, 1),
        borderBottomLeftHorizontal = Color.new(1, 0, 0, 1),
        borderBottomRightVertical = Color.new(1, 0, 0, 1),
        borderBottomRightHorizontal = Color.new(1, 0, 0, 1),
    }

    -- Función para actualizar los colores con base en las condiciones
    local function updateColorIf(flag, colorKey, newColor)
        if flag then
            colors[colorKey] = newColor
        end
    end

    -- Asignación de colores basada en las condiciones
    updateColorIf(isFloorDown, "floorDown", Color.new(0, 1, 0, 1))
    updateColorIf(isFloorDownLeft, "floorLeft", Color.new(0, 1, 0, 1))
    updateColorIf(isFloorDownRight, "floorRight", Color.new(0, 1, 0, 1))
    updateColorIf(isHookedLeft, "hookedLeft", Color.new(0, 1, 0, 1))
    updateColorIf(isHookedRight, "hookedRight", Color.new(0, 1, 0, 1))
    updateColorIf(isUp, "up", Color.new(0, 1, 0, 1))

    -- Actualización para bordes
    updateColorIf(isBorderUpLeftVertical, "borderUpLeftVertical", Color.new(0, 1, 0, 1))
    updateColorIf(isBorderUpRightVertical, "borderUpRightVertical", Color.new(0, 1, 0, 1))
    updateColorIf(isBorderUpLeftHorizontal, "borderUpLeftHorizontal", Color.new(0, 1, 0, 1))
    updateColorIf(isBorderUpRightHorizontal, "borderUpRightHorizontal", Color.new(0, 1, 0, 1))
    updateColorIf(isBorderBottomLeftVertical, "borderBottomLeftVertical", Color.new(0, 1, 0, 1))
    updateColorIf(isBorderBottomRightVertical, "borderBottomRightVertical", Color.new(0, 1, 0, 1))
    updateColorIf(isBorderBottomLeftHorizontal, "borderBottomLeftHorizontal", Color.new(0, 1, 0, 1))
    updateColorIf(isBorderBottomRightHorizontal, "borderBottomRightHorizontal", Color.new(0, 1, 0, 1))

    -- Dibujo de líneas
    local function drawLine(from, to, color)
        render:drawLine(from, to, color)
    end

    drawLine(from, rays.toDown, colors.floorDown)
    drawLine(from, rays.toDownLeft, colors.floorLeft)
    drawLine(from, rays.toDownRight, colors.floorRight)
    drawLine(from, rays.toUp, colors.up)
    drawLine(from, rays.toUpLeft, colors.hookedLeft)
    drawLine(from, rays.toUpRight, colors.hookedRight)

    drawLine(cross.upLeft.fromBorderVertical, cross.upLeft.toBorderVertical, colors.borderUpLeftVertical)
    drawLine(cross.upLeft.fromBorderHorizontal, cross.upLeft.toBorderHorizontal, colors.borderUpLeftHorizontal)
    drawLine(cross.upRight.fromBorderVertical, cross.upRight.toBorderVertical, colors.borderUpRightVertical)
    drawLine(cross.upRight.fromBorderHorizontal, cross.upRight.toBorderHorizontal, colors.borderUpRightHorizontal)

    drawLine(cross.bottomLeft.fromBorderVertical, cross.bottomLeft.toBorderVertical, colors.borderBottomLeftVertical)
    drawLine(cross.bottomLeft.fromBorderHorizontal, cross.bottomLeft.toBorderHorizontal, colors.borderBottomLeftHorizontal)
    drawLine(cross.bottomRight.fromBorderVertical, cross.bottomRight.toBorderVertical, colors.borderBottomRightVertical)
    drawLine(cross.bottomRight.fromBorderHorizontal, cross.bottomRight.toBorderHorizontal, colors.borderBottomRightHorizontal)
end

function onCollision(with)
end
