local Keys = {
    LEFT = "A",
    RIGHT = "D",
    UP = "W",
    DOWN = "S",
    JUMP = "SPACE"
}

local State = {
    IDLE = "idle",
    WALKING = "walking",
    JUMPING = "jumping",
    FALLING = "falling",
    HOOKED = "hooked",
    HANGLING = "hanging",
    HANGLING_TO_UP = "hanging_to_up",
    CROUCHED = "crouched",
    CROUCHING = "crouching",
}

local function createRayPositions(pos, this, lengthRay)
    local rays = {
        toDown = pos - this:AxisUp():getScaled(lengthRay),
        toUp = pos + this:AxisUp():getScaled(lengthRay),

        toLeft = pos + Vertex3D.new(0.4, 0, 0),
        toRight = pos + Vertex3D.new(-0.4, 0, 0),

        toDownLeft = pos - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.4, 0, 0),
        toDownRight = pos - this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.4, 0, 0),

        toUpLeft = pos + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(0.4, 0, 0),
        toUpRight = pos + this:AxisUp():getScaled(lengthRay) + Vertex3D.new(-0.4, 0, 0),

        toBottomLeft = pos + this:AxisUp():getScaled(lengthRay + 1.5) + Vertex3D.new(0.4, 0, 0),
        toBottomRight = pos + this:AxisUp():getScaled(lengthRay + 1.5) + Vertex3D.new(-0.4, 0, 0),

        cornerUpLeftCenter = pos + this:AxisUp():getScaled(1) + Vertex3D.new(0.3, 0, 0),
        cornerUpRightCenter = pos + this:AxisUp():getScaled(1) + Vertex3D.new(-0.3, 0, 0),

        cornerBottomLeftCenter = pos + this:AxisUp():getScaled(1 + 0.7) + Vertex3D.new(0.3, 0, 0),
        cornerBottomRightCenter = pos + this:AxisUp():getScaled(1 + 0.7) + Vertex3D.new(-0.3, 0, 0),
    }

    return rays
end

function getCrossBorders(center)
    local size = 0.6
    local halfSize = size * 0.4

    local fromBorderHorizontal = center - this:AxisRight():getScaled(halfSize)
    local toBorderHorizontal = center + this:AxisRight():getScaled(halfSize)

    local fromBorderDiagonal1 = center - (this:AxisUp() + this:AxisRight()):getScaled(halfSize)
    local toBorderDiagonal1 = center + (this:AxisUp() + this:AxisRight()):getScaled(halfSize)

    local fromBorderDiagonal2 = center - (this:AxisUp() - this:AxisRight()):getScaled(halfSize)
    local toBorderDiagonal2 = center + (this:AxisUp() - this:AxisRight()):getScaled(halfSize)

    return fromBorderHorizontal, toBorderHorizontal,
           fromBorderDiagonal1, toBorderDiagonal1, fromBorderDiagonal2, toBorderDiagonal2
end

function createBorderCross(cornerUpLeftCenter, cornerUpRightCenter, cornerBottomLeftCenter, cornerBottomRightCenter)
    -- Definimos las fronteras de los bordes superiores
    local fromBorderUpLeftHorizontal, toBorderUpLeftHorizontal,
          fromBorderUpLeftDiagonal1, toBorderUpLeftDiagonal1, fromBorderUpLeftDiagonal2, toBorderUpLeftDiagonal2 = getCrossBorders(cornerUpLeftCenter)

    local fromBorderUpRightHorizontal, toBorderUpRightHorizontal,
          fromBorderUpRightDiagonal1, toBorderUpRightDiagonal1, fromBorderUpRightDiagonal2, toBorderUpRightDiagonal2 = getCrossBorders(cornerUpRightCenter)

    -- Definimos las fronteras de los bordes inferiores
    local fromBorderBottomLeftHorizontal, toBorderBottomLeftHorizontal,
          fromBorderBottomLeftDiagonal1, toBorderBottomLeftDiagonal1, fromBorderBottomLeftDiagonal2, toBorderBottomLeftDiagonal2 = getCrossBorders(cornerBottomLeftCenter)

    local fromBorderBottomRightHorizontal, toBorderBottomRightHorizontal,
          fromBorderBottomRightDiagonal1, toBorderBottomRightDiagonal1, fromBorderBottomRightDiagonal2, toBorderBottomRightDiagonal2 = getCrossBorders(cornerBottomRightCenter)

    -- Bordes
    local borders = {
        upLeft = {
            fromBorderHorizontal = fromBorderUpLeftHorizontal,
            toBorderHorizontal = toBorderUpLeftHorizontal,
            fromBorderDiagonal1 = fromBorderUpLeftDiagonal1,
            toBorderDiagonal1 = toBorderUpLeftDiagonal1,
            fromBorderDiagonal2 = fromBorderUpLeftDiagonal2,
            toBorderDiagonal2 = toBorderUpLeftDiagonal2
        },
        upRight = {
            fromBorderHorizontal = fromBorderUpRightHorizontal,
            toBorderHorizontal = toBorderUpRightHorizontal,
            fromBorderDiagonal1 = fromBorderUpRightDiagonal1,
            toBorderDiagonal1 = toBorderUpRightDiagonal1,
            fromBorderDiagonal2 = fromBorderUpRightDiagonal2,
            toBorderDiagonal2 = toBorderUpRightDiagonal2
        },
        bottomLeft = {
            fromBorderHorizontal = fromBorderBottomLeftHorizontal,
            toBorderHorizontal = toBorderBottomLeftHorizontal,
            fromBorderDiagonal1 = fromBorderBottomLeftDiagonal1,
            toBorderDiagonal1 = toBorderBottomLeftDiagonal1,
            fromBorderDiagonal2 = fromBorderBottomLeftDiagonal2,
            toBorderDiagonal2 = toBorderBottomLeftDiagonal2
        },
        bottomRight = {
            fromBorderHorizontal = fromBorderBottomRightHorizontal,
            toBorderHorizontal = toBorderBottomRightHorizontal,
            fromBorderDiagonal1 = fromBorderBottomRightDiagonal1,
            toBorderDiagonal1 = toBorderBottomRightDiagonal1,
            fromBorderDiagonal2 = fromBorderBottomRightDiagonal2,
            toBorderDiagonal2 = toBorderBottomRightDiagonal2
        }
    }

    return borders
end

function UpdateCollisionFlags(input, from, rays, cross)
    local collisions = componentsManager:getComponentCollisions();
    local velocity = this:getLinearVelocity()

    -- Collision flags
    is.isFloorDown = collisions:isRayCollisionWith(from, rays.toDown, floor)
    is.isFloorDownLeft = collisions:isRayCollisionWith(from, rays.toDownLeft, floor)
    is.isFloorDownRight = collisions:isRayCollisionWith(from, rays.toDownRight, floor)
    is.isFloor =
        (
            ((is.isFloorDown and (is.isFloorDownLeft or is.isFloorDownRight)) and velocity.y <= 1)
        or
            ((is.isFloorDownLeft and is.isFloorDownRight) and velocity.y <= 1)
        )

    is.isUp = collisions:isRayCollisionWith(from, rays.toUp, floor)
    is.isLeft = collisions:isRayCollisionWith(from, rays.toLeft, floor)
    is.isRight = collisions:isRayCollisionWith(from, rays.toRight, floor)

    is.isHookedLeft = collisions:isRayCollisionWith(from, rays.toUpLeft, floor)
    is.isHookedRight = collisions:isRayCollisionWith(from, rays.toUpRight, floor)
    is.isHooked = (is.isHookedLeft or is.isHookedRight) and not is.isFloorDown

    -- corners up
    is.isCrossUpLeftHorizontal = collisions:isRayCollisionWith(cross.upLeft.fromBorderHorizontal, cross.upLeft.toBorderHorizontal, floor)
    is.isCrossUpLeftDiagonal1 = collisions:isRayCollisionWith(cross.upLeft.fromBorderDiagonal1, cross.upLeft.toBorderDiagonal1, floor)
    is.isCrossUpLeftDiagonal2 = collisions:isRayCollisionWith(cross.upLeft.fromBorderDiagonal2, cross.upLeft.toBorderDiagonal2, floor)
    is.isCornerUpLeft = is.isCrossUpLeftHorizontal and is.isCrossUpLeftDiagonal1 and is.isCrossUpLeftDiagonal2
    is.isFullFreeCornerUpLeft = not (is.isCrossUpLeftHorizontal or is.isCrossUpLeftDiagonal1 or is.isCrossUpLeftDiagonal2)

    is.isCrossUpRightHorizontal = collisions:isRayCollisionWith(cross.upRight.fromBorderHorizontal, cross.upRight.toBorderHorizontal, floor)
    is.isCrossUpRightDiagonal1 = collisions:isRayCollisionWith(cross.upRight.fromBorderDiagonal1, cross.upRight.toBorderDiagonal1, floor)
    is.isCrossUpRightDiagonal2 = collisions:isRayCollisionWith(cross.upRight.fromBorderDiagonal2, cross.upRight.toBorderDiagonal2, floor)
    is.isCornerUpRight = is.isCrossUpRightHorizontal and is.isCrossUpRightDiagonal1 and is.isCrossUpRightDiagonal2
    is.isFullFreeCornerUpRight = not (is.isCrossUpRightHorizontal or is.isCrossUpRightDiagonal1 or is.isCrossUpRightDiagonal1)

    -- corners bottom
    is.isCrossBottomLeftHorizontal = collisions:isRayCollisionWith(cross.bottomLeft.fromBorderHorizontal, cross.bottomLeft.toBorderHorizontal, floor)
    is.isCrossBottomLeftDiagonal1 = collisions:isRayCollisionWith(cross.bottomLeft.fromBorderDiagonal1, cross.bottomLeft.toBorderDiagonal1, floor)
    is.isCrossBottomLeftDiagonal2 = collisions:isRayCollisionWith(cross.bottomLeft.fromBorderDiagonal2, cross.bottomLeft.toBorderDiagonal2, floor)
    is.isCornerBottomLeft = is.isCrossBottomLeftHorizontal and is.isCrossBottomLeftDiagonal1 and is.isCrossBottomLeftDiagonal2
    is.isFullFreeCornerBottomLeft = not (is.isCrossBottomLeftHorizontal or is.isCrossBottomLeftDiagonal1 or is.isCrossBottomLeftDiagonal2)

    is.isCrossBottomRightHorizontal = collisions:isRayCollisionWith(cross.bottomRight.fromBorderHorizontal, cross.bottomRight.toBorderHorizontal, floor)
    is.isCrossBottomRightDiagonal1 = collisions:isRayCollisionWith(cross.bottomRight.fromBorderDiagonal1, cross.bottomRight.toBorderDiagonal1, floor)
    is.isCrossBottomRightDiagonal2 = collisions:isRayCollisionWith(cross.bottomRight.fromBorderDiagonal2, cross.bottomRight.toBorderDiagonal2, floor)
    is.isCornerBottomRight = is.isCrossBottomRightHorizontal and is.isCrossBottomRightDiagonal1 and is.isCrossBottomRightDiagonal2
    is.isFullFreeCornerBottomRight = not (is.isCrossBottomRightHorizontal or is.isCrossBottomRightDiagonal1 or is.isCrossBottomRightDiagonal2)

    is.isCrouched = is.isFloor and input:isCharPressed(Keys.DOWN)
end

function UpdateAnimationFromState()
    if (currentState == State.JUMPING) then
        this:setAnimationByName("Armature|POSE_JUMPING")
        this:setDrawOffset(Vertex3D.new(0, 0, 0))
    elseif (currentState == State.RUNNING) then
        this:setAnimationByName("Armature|RUNNING")
        this:setDrawOffset(Vertex3D.new(0, 0.3, 0))
    elseif (currentState == State.FALLING) then
        this:setAnimationByName("Armature|FALLING_IDLE")
        this:setDrawOffset(Vertex3D.new(0, 0, 0))
    elseif (currentState == State.IDLE) then
        this:setAnimationByName("Armature|IDLE")
        this:setDrawOffset(Vertex3D.new(0, 0.3, 0))
    elseif (currentState == State.HANGLING) then
        this:setAnimationByName("Armature|HANGLING_IDLE")
        this:setDrawOffset(Vertex3D.new(0, 0, 0))
    elseif (currentState == State.WALKING) then
        this:setAnimationByName("Armature|WALKING")
        this:setDrawOffset(Vertex3D.new(0, 0.3, 0))
    elseif (currentState == State.CROUCHING) then
        this:setAnimationByName("Armature|CROUCH_WALKING")
        this:setDrawOffset(Vertex3D.new(0, 0.7, 0))
    elseif (currentState == State.CROUCHED) then
        this:setAnimationByName("Armature|CROUCH_POSE")
        this:setDrawOffset(Vertex3D.new(0, 0.7, 0))
    elseif (currentState == State.HOOKED) then
        this:setAnimationByName("Armature|WALL_FALLING")
        this:setDrawOffset(Vertex3D.new(0, 0, 0))
    elseif (currentState == State.HANGLING_TO_UP) then
        this:setAnimationByName("Armature|HANGLING_TO_UP")
        this:setDrawOffset(Vertex3D.new(0, 0.08, 0))
    end
end

function setState(state)
    if  (currentState ~= State.CROUCHED and currentState ~= State.CROUCHING) and (state == State.CROUCHED or state == State.CROUCHING)    then
        print("Crouching!")
        this:disableSimulationCollider()
        this:setCapsuleColliderSize(0.25, 0.5)
        this:setPosition(this:getPosition() + Vertex3D.new(0, -0.3, 0))
        this:UpdateShapeCollider();
        this:enableSimulationCollider()
    elseif (currentState == State.CROUCHED or currentState == State.CROUCHING) and (state ~= State.CROUCHED and (state ~= State.CROUCHING)) then
        print("Standup!")
        this:disableSimulationCollider()
        this:setCapsuleColliderSize(0.25, 1)
        this:setPosition(this:getPosition() + Vertex3D.new(0, 0.25, 0))
        this:UpdateShapeCollider();
        this:enableSimulationCollider()
    end

    currentState = state
end
------------------------------------------------------------------------------------------------------------------------
function onStart()
    currentState = State.IDLE

    floor = brakeza:getSceneObjectByLabel("floor")

    countUnblockableFrames = 0

    rotationLeft = M3:getMatrixRotationForEulerAngles(180, 0, 0);
    rotationRight = M3:getMatrixRotationForEulerAngles(0, 0, 0);
    sideRotation = rotationLeft;

    cameraOffsetPosition = this:AxisForward():getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0)

    offsetHanglingToUp = Vertex3D.new(-0.6397, 1.8052, 0);
    originHanglingToUp = Vertex3D.new(0, 0, 0)

    is = {
        isFloor = false,
        isHooked = false,

        isUp = false,
        isHookedLeft = false,
        isHookedRight = false,
        isFloorDown = false,
        isFloorDownLeft = false,
        isFloorDownRight = false,

        isCrossUpLeftHorizontal = false,
        isCrossUpRightHorizontal = false,

        isCrossUpLeftDiagonal1 = false,
        isCrossUpLeftDiagonal2 = false,
        isCrossUpRightDiagonal1 = false,
        isCrossUpRightDiagonal2 = false,

        isCrossBottomLeftDiagonal1 = false,
        isCrossBottomRightDiagonal2 = false,

        isCornerUpLeft = false,
        isCornerUpRight = false,

        isFullFreeCornerUpLeft = false,
        isFullFreeCornerUpRight = false,
        isFullFreeCornerBottomLeft = false,
        isFullFreeCornerBottomRight = false,

        isBlockedByHook = false,

        isHanglingToUp = false
    }
end

function decreaseCounterBlock()
    countUnblockableFrames = countUnblockableFrames - 1
    if countUnblockableFrames < 0 then
        countUnblockableFrames = 0
    end
end
function onUpdate()
    local position = this:getPosition()
    local rays = createRayPositions(position, this, lengthRay)
    local cross = createBorderCross(rays.cornerUpLeftCenter, rays.cornerUpRightCenter, rays.cornerBottomLeftCenter, rays.cornerBottomRightCenter)

    local input = componentsManager:getComponentInput();
    UpdateCollisionFlags(input, position, rays, cross)

    decreaseCounterBlock()

    this:setRotation(sideRotation)
    local dt = brakeza:getDeltaTime()

    handleFloorMovement(input, dt)
    handleHanglingToToUp()
    handleHook(input, dt)

    UpdateAnimationFromState()

    debug(position, rays, cross)
end

function handleHanglingToToUp()
    if is.isHanglingToUp and this:isAnimationEnds() then
        print("fin!")
        is.isHanglingToUp = false
        is.isBlockedByHook = false
        this:setLoop(true)
        setState(State.IDLE)
        this:setPosition(originHanglingToUp)
        this:UpdateShapeCollider();
        this:enableSimulationCollider()
    end
end

function handleFloorMovement(input, dt)
    if is.isHanglingToUp then
        return
    end

    if is.isBlockedByHook then
        return
    end

    if is.isHooked and not is.isFloor then
        return
    end

    local velocity = this:getLinearVelocity() -- Obtiene la velocidad actual

    if is.isFloor then
        if not is.isCrouched then
            if input:isCharFirstEventDown(Keys.JUMP) then
                local jumpVector = Vertex3D.new(0, jumpForce, 0)
                this:applyCentralImpulse(jumpVector)
                setState(State.JUMPING)
                print("Jump!")
                return
            end

            if input:isCharPressed(Keys.LEFT) then
                print("moving left")
                setState(State.WALKING)
                this:enableSimulationCollider()
                this:applyCentralForce(Vertex3D.new(-speed * dt * 1000, 0, 0))
                sideRotation = rotationLeft
            end

            if input:isCharPressed(Keys.RIGHT) then
                print("moving right")
                setState(State.WALKING)
                this:enableSimulationCollider()
                this:applyCentralForce(Vertex3D.new(speed * dt * 1000, 0, 0))
                sideRotation = rotationRight
            end

            if math.abs(velocity.x) > maxSpeed then
                velocity.x = maxSpeed * (velocity.x > 0 and 1 or -1)
                this:setLinearVelocity(velocity)
            end

            if math.abs(velocity.x) <= 2.25 then
                setState(State.WALKING)
                if math.abs(velocity.x) <= 0.1 then
                    setState(State.IDLE)
                end
            else
                setState(State.RUNNING)
            end
        else
            if input:isCharPressed(Keys.DOWN) then
                print("crouch when crouched")
                setState(State.CROUCHED)
            end

            if math.abs(velocity.x) > maxSpeed/4 then
                velocity.x = maxSpeed/4 * (velocity.x > 0 and 1 or -1)
                this:setLinearVelocity(velocity)
            end

            if input:isCharPressed(Keys.LEFT) then
                print("moving crouch left")
                setState(State.CROUCHING)
                this:enableSimulationCollider()
                this:applyCentralForce(Vertex3D.new(-speed * 3 * dt * 1000, 0, 0))
                sideRotation = rotationLeft
            end

            if input:isCharPressed(Keys.RIGHT) then
                print("moving crouch right")
                setState(State.CROUCHING)
                this:enableSimulationCollider()
                this:applyCentralForce(Vertex3D.new(speed * 3 * dt * 1000, 0, 0))
                sideRotation = rotationRight
            end
        end
    else
        if input:isCharPressed(Keys.LEFT) then
            sideRotation = rotationLeft
            this:applyCentralForce(Vertex3D.new(-speed * airControlFactor * dt * 1000, 0, 0))
        end

        if input:isCharPressed(Keys.RIGHT) then
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
            setState(State.FALLING)
        end
    end
end

function handleHook(input, dt)
    if is.isHanglingToUp then
        return
    end

    local v = this:getLinearVelocity()

    if is.isBlockedByHook then
        if input:isCharFirstEventDown(Keys.JUMP) then
            print("Freedom border!")
            this:setLinearVelocity(Vertex3D.new(0, -1, 0))
            this:enableSimulationCollider()
            countUnblockableFrames = 50
            is.isBlockedByHook = false
            return
        end
        if input:isCharFirstEventDown(Keys.UP) then
            setState(State.HANGLING_TO_UP)
            if (sideRotation == rotationLeft) then
                originHanglingToUp = this:getPosition() + offsetHanglingToUp
            else
                originHanglingToUp = this:getPosition() + Vertex3D.new(-offsetHanglingToUp.x, offsetHanglingToUp.y, offsetHanglingToUp.z)
            end
            print("Climb to top!")
            is.isHanglingToUp = true
            this:disableSimulationCollider();
            this:setLoop(false)
            return
        end
    else
        if is.isCornerUpRight and is.isFullFreeCornerBottomRight and countUnblockableFrames == 0 and not is.isFloor and is.isFullFreeCornerUpLeft and v.y < 0 and sideRotation == rotationLeft then
            print("Cross Up Right!")
            setState(State.HANGLING)
            this:disableSimulationCollider()
            is.isBlockedByHook = true
            sideRotation = rotationLeft
            return
        end

        if is.isCornerUpLeft and is.isFullFreeCornerBottomLeft and countUnblockableFrames == 0 and not is.isFloor and is.isFullFreeCornerUpRight and v.y < 0  and sideRotation == rotationRight then
            print("Cross Up Left!")
            setState(State.HANGLING)
            this:disableSimulationCollider()
            is.isBlockedByHook = true
            sideRotation = rotationRight
            return
        end

        local fallingFriction = 0.85;
        if is.isHookedLeft and is.isFloorDownLeft and not is.isFloorDown and not is.isFloorDownRight and input:isCharPressed(Keys.RIGHT) then
                print("stop r")
                sideRotation = rotationLeft
                setState(State.HOOKED)
                local v = this:getLinearVelocity();
                v.x = 0;
                v.y = v.y * fallingFriction;
                v.z = 0;
                this:setLinearVelocity(v);
            end
            if is.isHookedLeft and not is.isFloorDown and input:isCharPressed(Keys.LEFT) then
                print("release stop r")
                this:applyCentralForce(Vertex3D.new(-speed * airControlFactor * dt * 1000, 0, 0))
                setState(State.FALLING)
            end

            if is.isHookedRight and is.isFloorDownRight and not is.isFloorDown and not is.isFloorDownLeft and input:isCharPressed(Keys.LEFT) then
                setState(State.HOOKED)
                sideRotation = rotationRight
                print("stop l")
                local v = this:getLinearVelocity();
                v.x = 0;
                v.y = v.y * fallingFriction;
                v.z = 0;
                this:setLinearVelocity(v);
            end

            if is.isHookedRight and not is.isFloorDown and input:isCharPressed("F") then
                setState(State.FALLING)
                print("release stop l")
            end

            if is.isHookedLeft and not is.isFloorDown then
                if input:isCharFirstEventDown(Keys.JUMP) and not input:isCharPressed(Keys.RIGHT) and not is.isFloor then
                    setState(State.JUMPING)
                    this:enableSimulationCollider()
                    local jumpVector = Vertex3D.new(-100, jumpForce, 0)
                    this:applyCentralImpulse(jumpVector)
                    print("Jump hook l!")
                end
            end
            if is.isHookedRight and not is.isFloorDown then
                if input:isCharFirstEventDown(Keys.JUMP) and not input:isCharPressed(Keys.LEFT) and not is.isFloor then
                    this:sleepCollider()
                    local jumpVector = Vertex3D.new(100, jumpForce, 0)
                    this:applyCentralImpulse(jumpVector)
                    print("Jump hook r!")
                end
            end
    end

    if (is.isHookedRight and is.isHookedLeft and is.isUp) then
        return
    end
end

function debug(from, rays, cross)
    local render = componentsManager:getComponentRender()

    local colors = {
        floorLeft = Color.new(1, 0, 0, 1),
        floorRight = Color.new(1, 0, 0, 1),
        floorDown = Color.new(1, 0, 0, 1),
        up = Color.new(1, 0, 0, 1),
        left = Color.new(1, 0, 0, 1),
        right = Color.new(1, 0, 0, 1),
        hookedLeft = Color.new(1, 0, 0, 1),
        hookedRight = Color.new(1, 0, 0, 1),
        borderUpLeftHorizontal = Color.new(1, 0, 0, 1),
        borderUpLeftDiagonal1 =  Color.new(1, 0, 0, 1),
        borderUpLeftDiagonal2 =  Color.new(1, 0, 0, 1),
        borderUpRightHorizontal = Color.new(1, 0, 0, 1),
        borderUpRightDiagonal1 =  Color.new(1, 0, 0, 1),
        borderUpRightDiagonal2 =  Color.new(1, 0, 0, 1),
        borderBottomLeftHorizontal = Color.new(1, 0, 0, 1),
        borderBottomLeftDiagonal1 =  Color.new(1, 0, 0, 1),
        borderBottomLeftDiagonal2 =  Color.new(1, 0, 0, 1),
        borderBottomRightHorizontal = Color.new(1, 0, 0, 1),
        borderBottomRightDiagonal1 =  Color.new(1, 0, 0, 1),
        borderBottomRightDiagonal2 =  Color.new(1, 0, 0, 1),
    }

    -- Función para actualizar los colores con base en las condiciones
    local function updateColorIf(flag, colorKey, newColor)
        if flag then
            colors[colorKey] = newColor
        end
    end

    -- Asignación de colores basada en las condiciones
    updateColorIf(is.isFloorDown, "floorDown", Color.new(0, 1, 0, 1))
    updateColorIf(is.isFloorDownLeft, "floorLeft", Color.new(0, 1, 0, 1))
    updateColorIf(is.isFloorDownRight, "floorRight", Color.new(0, 1, 0, 1))
    updateColorIf(is.isHookedLeft, "hookedLeft", Color.new(0, 1, 0, 1))
    updateColorIf(is.isHookedRight, "hookedRight", Color.new(0, 1, 0, 1))
    updateColorIf(is.isUp, "up", Color.new(0, 1, 0, 1))
    updateColorIf(is.isLeft, "left", Color.new(0, 1, 0, 1))
    updateColorIf(is.isRight, "right", Color.new(0, 1, 0, 1))

    -- Actualización para bordes
    updateColorIf(is.isCrossUpLeftHorizontal, "borderUpLeftHorizontal", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossUpRightHorizontal, "borderUpRightHorizontal", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossBottomLeftHorizontal, "borderBottomLeftHorizontal", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossBottomRightHorizontal, "borderBottomRightHorizontal", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossUpLeftDiagonal1, "borderUpLeftDiagonal1", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossUpLeftDiagonal2, "borderUpLeftDiagonal2", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossUpRightDiagonal1, "borderUpRightDiagonal1", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossUpRightDiagonal2, "borderUpRightDiagonal2", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossBottomLeftDiagonal1, "borderBottomLeftDiagonal1", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossBottomLeftDiagonal2, "borderBottomLeftDiagonal2", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossBottomRightDiagonal1, "borderBottomRightDiagonal1", Color.new(0, 1, 0, 1))
    updateColorIf(is.isCrossBottomRightDiagonal2, "borderBottomRightDiagonal2", Color.new(0, 1, 0, 1))

    -- Dibujo de líneas
    local function drawLine(from, to, color)
        render:drawLine(from, to, color)
    end

    drawLine(from, rays.toDown, colors.floorDown)
    drawLine(from, rays.toDownLeft, colors.floorLeft)
    drawLine(from, rays.toDownRight, colors.floorRight)
    drawLine(from, rays.toUp, colors.up)
    drawLine(from, rays.toLeft, colors.left)
    drawLine(from, rays.toRight, colors.right)
    drawLine(from, rays.toUpLeft, colors.hookedLeft)
    drawLine(from, rays.toUpRight, colors.hookedRight)

    drawLine(cross.upLeft.fromBorderHorizontal, cross.upLeft.toBorderHorizontal, colors.borderUpLeftHorizontal)
    drawLine(cross.upLeft.fromBorderDiagonal1, cross.upLeft.toBorderDiagonal1, colors.borderUpLeftDiagonal1)
    drawLine(cross.upLeft.fromBorderDiagonal2, cross.upLeft.toBorderDiagonal2, colors.borderUpLeftDiagonal2)

    drawLine(cross.upRight.fromBorderHorizontal, cross.upRight.toBorderHorizontal, colors.borderUpRightHorizontal)
    drawLine(cross.upRight.fromBorderDiagonal1, cross.upRight.toBorderDiagonal1, colors.borderUpRightDiagonal1)
    drawLine(cross.upRight.fromBorderDiagonal2, cross.upRight.toBorderDiagonal2, colors.borderUpRightDiagonal2)

    drawLine(cross.bottomLeft.fromBorderHorizontal, cross.bottomLeft.toBorderHorizontal, colors.borderBottomLeftHorizontal)
    drawLine(cross.bottomLeft.fromBorderDiagonal1, cross.bottomLeft.toBorderDiagonal1, colors.borderBottomLeftDiagonal1)
    drawLine(cross.bottomLeft.fromBorderDiagonal2, cross.bottomLeft.toBorderDiagonal2, colors.borderBottomLeftDiagonal2)

    drawLine(cross.bottomRight.fromBorderHorizontal, cross.bottomRight.toBorderHorizontal, colors.borderBottomRightHorizontal)
    drawLine(cross.bottomRight.fromBorderDiagonal1, cross.bottomRight.toBorderDiagonal1, colors.borderBottomRightDiagonal1)
    drawLine(cross.bottomRight.fromBorderDiagonal2, cross.bottomRight.toBorderDiagonal2, colors.borderBottomRightDiagonal2)
end

function onCollision(with)
end