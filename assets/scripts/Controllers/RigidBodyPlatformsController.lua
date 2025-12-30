local Keys = {
    LEFT = "A",
    RIGHT = "D",
    UP = "W",
    DOWN = "S",
    JUMP = "SPACE",
    DASH = "Left Shift"
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
    local sign = (sideRotation == rotationLeft) and 1 or -1
    local upAway, dashLengthChecker = 1.2, 0
    local axisUp = this:AxisUp()
    local axisRight = this:AxisRight()
    local axisUpScaled = axisUp:getScaled(1)
    local axisUpScaledRay = axisUp:getScaled(lengthRay)

    -- Posiciones base
    local fromHanglingToUp = pos + Vertex3D.new(sign * offsetHanglingToUp.x, offsetHanglingToUp.y - 0.6, offsetHanglingToUp.z)
    local toHanglingToUp = fromHanglingToUp + axisUpScaled
    local fromDash = pos + axisRight:getScaled(sign * dashLengthChecker)
    local toDash = fromDash + axisUpScaled + axisRight:getScaled(sign * 2)
    local fromDash2 = fromDash - axisUp:getScaled(0.5)
    local toDash2 = fromDash2 + axisRight:getScaled(sign * 2)
    local toDash3 = fromDash + axisRight:getScaled(sign * 2)

    -- Definir las posiciones de los rayos
    return {
        fromUp = pos,
        toDown = pos - axisUpScaledRay,
        toUp = pos + axisUpScaledRay,

        toLeft = pos + axisUp:getScaled(0.9) + Vertex3D.new(0.4, 0, 0),
        toRight = pos + axisUp:getScaled(0.9) + Vertex3D.new(-0.4, 0, 0),

        toDownLeft = pos - axisUpScaledRay + Vertex3D.new(0.4, 0, 0),
        toDownRight = pos - axisUpScaledRay + Vertex3D.new(-0.4, 0, 0),

        fromUpLeft = pos + axisUp:getScaled(upAway),
        fromUpRight = pos + axisUp:getScaled(upAway),
        toUpLeft = pos + axisUp:getScaled(upAway) + Vertex3D.new(0.4, 0, 0),
        toUpRight = pos + axisUp:getScaled(upAway) + Vertex3D.new(-0.4, 0, 0),

        toBottomLeft = pos + axisUp:getScaled(lengthRay + 1.5) + Vertex3D.new(0.4, 0, 0),
        toBottomRight = pos + axisUp:getScaled(lengthRay + 1.5) + Vertex3D.new(-0.4, 0, 0),

        cornerUpLeftCenter = pos + axisUpScaled + Vertex3D.new(0.3, 0, 0),
        cornerUpRightCenter = pos + axisUpScaled + Vertex3D.new(-0.3, 0, 0),

        cornerBottomLeftCenter = pos + axisUp:getScaled(1.7) + Vertex3D.new(0.3, 0, 0),
        cornerBottomRightCenter = pos + axisUp:getScaled(1.7) + Vertex3D.new(-0.3, 0, 0),

        fromHanglingToUp = fromHanglingToUp, toHanglingToUp = toHanglingToUp,
        fromDash = fromDash, toDash = toDash,
        fromDash2 = fromDash2, toDash2 = toDash2,
        fromDash3 = fromDash, toDash3 = toDash3
    }
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

    return fromBorderHorizontal, toBorderHorizontal, fromBorderDiagonal1, toBorderDiagonal1, fromBorderDiagonal2, toBorderDiagonal2
end

local function createBorderCross(cornerUpLeftCenter, cornerUpRightCenter, cornerBottomLeftCenter, cornerBottomRightCenter)
    local function getBorders(corner)
        local fromH, toH, fromD1, toD1, fromD2, toD2 = getCrossBorders(corner)
        return {
            fromBorderHorizontal = fromH, toBorderHorizontal = toH,
            fromBorderDiagonal1 = fromD1, toBorderDiagonal1 = toD1,
            fromBorderDiagonal2 = fromD2, toBorderDiagonal2 = toD2
        }
    end

    return {
        upLeft = getBorders(cornerUpLeftCenter),
        upRight = getBorders(cornerUpRightCenter),
        bottomLeft = getBorders(cornerBottomLeftCenter),
        bottomRight = getBorders(cornerBottomRightCenter)
    }
end

function UpdateCollisionFlags(input, from, rays, cross)
    local collisions = Components:Collisions();
    local velocity = this:getLinearVelocity()

    -- Collision flags
    is.isFloorDown = collisions:isRayCollisionWith(from, rays.toDown, floor)
    is.isFloorDownLeft = collisions:isRayCollisionWith(from, rays.toDownLeft, floor)
    is.isFloorDownRight = collisions:isRayCollisionWith(from, rays.toDownRight, floor)
    is.isFloor = (((is.isFloorDown and (is.isFloorDownLeft or is.isFloorDownRight)) and velocity.y <= 1) or
                 ((is.isFloorDownLeft and is.isFloorDownRight) and velocity.y <= 1))
    is.isUp = collisions:isRayCollisionWith(rays.fromUp, rays.toUp, floor)
    is.isLeft = collisions:isRayCollisionWith(rays.fromUp, rays.toLeft, floor)
    is.isRight = collisions:isRayCollisionWith(rays.fromUp, rays.toRight, floor)

    is.isHookedLeft = collisions:isRayCollisionWith(rays.fromUpLeft, rays.toUpLeft, floor)
    is.isHookedRight = collisions:isRayCollisionWith(rays.fromUpRight, rays.toUpRight, floor)
    if (sideRotation == rotationLeft) then
        is.isHooked = (is.isHookedLeft and not is.isHookedRight) and not is.isFloorDown and input:isCharPressed(Keys.LEFT)
    else
        is.isHooked = (not is.isHookedLeft and is.isHookedRight) and not is.isFloorDown and input:isCharPressed(Keys.RIGHT)
    end

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

    is.isHanglingToUpCanceled = collisions:isRayCollisionWith(rays.fromHanglingToUp, rays.toHanglingToUp, floor)

    is.isDashCanceled = (collisions:isRayCollisionWith(rays.fromDash, rays.toDash, floor) or
                        collisions:isRayCollisionWith(rays.fromDash2, rays.toDash2, floor) or
                        collisions:isRayCollisionWith(rays.fromDash3, rays.toDash3, floor))

    is.isCrouched = (is.isFloor and input:isCharPressed(Keys.DOWN)) or
                    (is.isFloor and is.isUp )
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
        this:setCapsuleColliderSize(0.2, 0.5)
        this:setRotation(M3:getMatrixRotationForEulerAngles(180, 0, 0))
        this:setPosition(this:getPosition() + Vertex3D.new(0, -0.3, 0))
        this:UpdateShapeCollider();
        this:EnableSimulationCollider()
    elseif (currentState == State.CROUCHED or currentState == State.CROUCHING) and (state ~= State.CROUCHED and (state ~= State.CROUCHING)) then
        print("Standup!")
        this:DisableSimulationCollider()
        this:setCapsuleColliderSize(0.2, 1)
        this:setRotation(M3:getMatrixRotationForEulerAngles(180, 0, 0))
        this:setPosition(this:getPosition() + Vertex3D.new(0, 0.2, 0))
        this:UpdateShapeCollider();
        this:EnableSimulationCollider()
    end

    currentState = state

end
------------------------------------------------------------------------------------------------------------------------
function onStart()
    currentState = State.IDLE

    floor = Brakeza:getObjectByName("floor")

    countUnblockableFrames = 0

    rotationLeft = M3:getMatrixRotationForEulerAngles(180, 0, 0);
    rotationRight = M3:getMatrixRotationForEulerAngles(0, 0, 0);
    sideRotation = rotationLeft;
    this:setRotation(sideRotation)

    cameraOffsetPosition = this:AxisForward():getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0)

    offsetHanglingToUp = Vertex3D.new(-0.6397, 1.8052, 0);
    destinyHanglingToUp = Vertex3D.new(0, 0, 0)

    doubleJumpUsed = false
    dashUsed = false

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

    local input = Components:Input();
    UpdateCollisionFlags(input, position, rays, cross)

    decreaseCounterBlock()

    this:setRotation(sideRotation)
    local dt = Brakeza:getDeltaTime()

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
        this:setRotation(M3:getMatrixRotationForEulerAngles(180, 0, 0))
        this:setPosition(destinyHanglingToUp)
        this:UpdateShapeCollider();
        this:EnableSimulationCollider()
    end
end

function handleFloorMovement(input, dt)
    if is.isHanglingToUp or is.isBlockedByHook or (is.isHooked and not is.isFloor) then
        return
    end

    local velocity = this:getLinearVelocity()
    local avoidMaxLimit = false

    if is.isFloor then
        doubleJumpUsed = false
        dashUsed = false

        if not is.isCrouched then
            if input:isCharFirstEventDown(Keys.JUMP) then
                local jumpVector = Vertex3D.new(0, jumpForce, 0)
                this:ApplyCentralImpulse(jumpVector)
                setState(State.JUMPING)
                print("Jump!")
                return
            end

            if input:isCharPressed(Keys.LEFT) then
                print("moving left")
                setState(State.WALKING)
                this:EnableSimulationCollider()
                this:ApplyCentralImpulse(Vertex3D.new(-speed * dt * 1000, 0, 0))
                sideRotation = rotationLeft
                if input:isCharFirstEventDown(Keys.DASH) and not is.isDashCanceled then
                    this:ApplyCentralImpulse(Vertex3D.new(-speed * 5 * dt * 1000, 0, 0))
                    print("moving left DASH")
                    dashUsed = true
                    avoidMaxLimit = true
                end
            end

            if input:isCharPressed(Keys.RIGHT) then
                print("moving right")
                setState(State.WALKING)
                this:EnableSimulationCollider()
                this:ApplyCentralImpulse(Vertex3D.new(speed * dt * 1000, 0, 0))
                sideRotation = rotationRight
                if input:isCharFirstEventDown(Keys.DASH) and not is.isDashCanceled then
                    this:ApplyCentralImpulse(Vertex3D.new(speed * 5 * dt * 1000, 0, 0))
                    print("moving left DASH")
                    dashUsed = true
                    avoidMaxLimit = true
                end
            end

            if math.abs(velocity.x) > maxSpeed and not avoidMaxLimit then
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
        end

        if is.isCrouched then
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
                this:EnableSimulationCollider()
                this:ApplyCentralImpulse(Vertex3D.new(-speed * 3 * dt * 1000, 0, 0))
                sideRotation = rotationLeft
            end

            if input:isCharPressed(Keys.RIGHT) then
                print("moving crouch right")
                setState(State.CROUCHING)
                this:EnableSimulationCollider()
                this:ApplyCentralImpulse(Vertex3D.new(speed * 3 * dt * 1000, 0, 0))
                sideRotation = rotationRight
            end
        end
    end
    if not is.isFloor then

        if input:isCharPressed(Keys.LEFT) then
            sideRotation = rotationLeft
            this:ApplyCentralImpulse(Vertex3D.new(-speed * airControlFactor * dt * 1000, 0, 0))
            if input:isCharFirstEventDown(Keys.DASH) and not dashUsed and not is.isDashCanceled then
                this:ApplyCentralImpulse(Vertex3D.new(-speed * 5 * dt * 1000, 0, 0))
                print("left air DASH")
                dashUsed = true
                avoidMaxLimit = true
            end
        end

        if input:isCharPressed(Keys.RIGHT) then
            sideRotation = rotationRight
            this:ApplyCentralImpulse(Vertex3D.new(speed * airControlFactor * dt * 1000, 0, 0))
            if input:isCharFirstEventDown(Keys.DASH) and not dashUsed and not is.isDashCanceled then
                this:ApplyCentralImpulse(Vertex3D.new(speed * 5 * dt * 1000, 0, 0))
                print("right air DASH")
                dashUsed = true
                avoidMaxLimit = true
            end
        end

        if input:isCharFirstEventDown(Keys.JUMP) and not doubleJumpUsed then
            this:setLinearVelocity(Vertex3D.new(velocity.x, 0, velocity.z))
            doubleJumpUsed = true;
            local jumpVector = Vertex3D.new(0, jumpForce, 0)
            this:ApplyCentralImpulse(jumpVector)
            setState(State.JUMPING)
            print("Jump!")
            return
        end

        -- Reducir la velocidad en el aire si es muy alta
        local maxAirSpeed = 3.0
        if math.abs(velocity.x) > maxAirSpeed and not avoidMaxLimit then
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
            this:EnableSimulationCollider()
            countUnblockableFrames = 50
            is.isBlockedByHook = false
            return
        end
        if input:isCharFirstEventDown(Keys.UP) and not is.isHanglingToUpCanceled then
            setState(State.HANGLING_TO_UP)
            if (sideRotation == rotationLeft) then
                destinyHanglingToUp = this:getPosition() + offsetHanglingToUp
            else
                destinyHanglingToUp = this:getPosition() + Vertex3D.new(-offsetHanglingToUp.x, offsetHanglingToUp.y, offsetHanglingToUp.z)
            end
            print("Climb to top!")
            is.isHanglingToUp = true
            this:disableSimulationCollider();
            this:setLoop(false)
            return
        end
    else
        if is.isCornerUpRight and is.isFullFreeCornerBottomRight and countUnblockableFrames == 0 and not is.isFloor and is.isFullFreeCornerUpLeft and v.y < 0 and not is.isHanglingToUpCanceled and sideRotation ==  rotationLeft then
            print("Cross Up Right!")
            setState(State.HANGLING)
            this:disableSimulationCollider()
            is.isBlockedByHook = true
            sideRotation = rotationLeft
            doubleJumpUsed = true
            return
        end

        if is.isCornerUpLeft and is.isFullFreeCornerBottomLeft and countUnblockableFrames == 0 and not is.isFloor and is.isFullFreeCornerUpRight and v.y < 0 and not is.isHanglingToUpCanceled and sideRotation == rotationRight then
            print("Cross Up Left!")
            setState(State.HANGLING)
            this:disableSimulationCollider()
            is.isBlockedByHook = true
            sideRotation = rotationRight
            doubleJumpUsed = true
            return
        end

        local fallingFriction = 0.85;
        if is.isHookedLeft and is.isFloorDownLeft and not is.isFloorDown and not is.isFloorDownRight and input:isCharPressed(Keys.RIGHT) and v.y <= 0  then
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
            this:ApplyCentralImpulse(Vertex3D.new(-speed * airControlFactor * dt * 1000, 0, 0))
            setState(State.FALLING)
        end

        if is.isHookedRight and is.isFloorDownRight and not is.isFloorDown and not is.isFloorDownLeft and input:isCharPressed(Keys.LEFT) and v.y <= 0 then
            setState(State.HOOKED)
            sideRotation = rotationRight
            print("stop l")
            local v = this:getLinearVelocity();
            v.x = 0;
            v.y = v.y * fallingFriction;
            v.z = 0;
            this:setLinearVelocity(v);
        end

        if is.isHookedRight and not is.isFloorDown and input:isCharPressed(Keys.RIGHT) then
            setState(State.FALLING)
            print("release stop l")
        end

        if is.isHookedLeft and not is.isFloorDown then
            if input:isCharFirstEventDown(Keys.JUMP) and not input:isCharPressed(Keys.RIGHT) and not is.isFloor then
                setState(State.JUMPING)
                this:EnableSimulationCollider()
                local jumpVector = Vertex3D.new(-100, jumpForce, 0)
                this:ApplyCentralImpulse(jumpVector)
                print("Jump hook l!")
            end
        end
        if is.isHookedRight and not is.isFloorDown then
            if input:isCharFirstEventDown(Keys.JUMP) and not input:isCharPressed(Keys.LEFT) and not is.isFloor then
                this:sleepCollider()
                local jumpVector = Vertex3D.new(100, jumpForce, 0)
                this:ApplyCentralImpulse(jumpVector)
                print("Jump hook r!")
            end
        end
    end

end

function debug(from, rays, cross)
    local render = Components:Render()

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
        borderUpLeftDiagonal1 = Color.new(1, 0, 0, 1),
        borderUpLeftDiagonal2 = Color.new(1, 0, 0, 1),
        borderUpRightHorizontal = Color.new(1, 0, 0, 1),
        borderUpRightDiagonal1 = Color.new(1, 0, 0, 1),
        borderUpRightDiagonal2 = Color.new(1, 0, 0, 1),
        borderBottomLeftHorizontal = Color.new(1, 0, 0, 1),
        borderBottomLeftDiagonal1 = Color.new(1, 0, 0, 1),
        borderBottomLeftDiagonal2 = Color.new(1, 0, 0, 1),
        borderBottomRightHorizontal = Color.new(1, 0, 0, 1),
        borderBottomRightDiagonal1 = Color.new(1, 0, 0, 1),
        borderBottomRightDiagonal2 = Color.new(1, 0, 0, 1),
        hanglingToUpCanceled = Color.new(1, 0, 0, 1),
        dashCanceled = Color.new(1, 0, 0, 1)
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
    updateColorIf(is.isHanglingToUpCanceled, "hanglingToUpCanceled", Color.new(0, 1, 0, 1))
    updateColorIf(is.isDashCanceled, "dashCanceled", Color.new(0, 1, 0, 1))

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
        render:DrawLine(from, to, color)
    end

    drawLine(from, rays.toDown, colors.floorDown)
    drawLine(from, rays.toDownLeft, colors.floorLeft)
    drawLine(from, rays.toDownRight, colors.floorRight)
    drawLine(rays.fromUp, rays.toUp, colors.up)
    drawLine(from, rays.toLeft, colors.left)
    drawLine(from, rays.toRight, colors.right)

    drawLine(cross.upRight.fromBorderHorizontal, cross.upRight.toBorderHorizontal, colors.borderUpRightHorizontal)
    drawLine(cross.upRight.fromBorderDiagonal1, cross.upRight.toBorderDiagonal1, colors.borderUpRightDiagonal1)
    drawLine(cross.upRight.fromBorderDiagonal2, cross.upRight.toBorderDiagonal2, colors.borderUpRightDiagonal2)

    drawLine(cross.bottomLeft.fromBorderHorizontal, cross.bottomLeft.toBorderHorizontal, colors.borderBottomLeftHorizontal)
    drawLine(cross.bottomLeft.fromBorderDiagonal1, cross.bottomLeft.toBorderDiagonal1, colors.borderBottomLeftDiagonal1)
    drawLine(cross.bottomLeft.fromBorderDiagonal2, cross.bottomLeft.toBorderDiagonal2, colors.borderBottomLeftDiagonal2)

    drawLine(cross.bottomRight.fromBorderHorizontal, cross.bottomRight.toBorderHorizontal, colors.borderBottomRightHorizontal)
    drawLine(cross.bottomRight.fromBorderDiagonal1, cross.bottomRight.toBorderDiagonal1, colors.borderBottomRightDiagonal1)
    drawLine(cross.bottomRight.fromBorderDiagonal2, cross.bottomRight.toBorderDiagonal2, colors.borderBottomRightDiagonal2)

    drawLine(rays.fromUpLeft, rays.toUpLeft, colors.hookedLeft)
    drawLine(rays.fromUpRight, rays.toUpRight, colors.hookedRight)

    drawLine(rays.fromHanglingToUp, rays.toHanglingToUp, colors.hanglingToUpCanceled)

    drawLine(rays.fromDash, rays.toDash, colors.dashCanceled)
    drawLine(rays.fromDash2, rays.toDash2, colors.dashCanceled)
    drawLine(rays.fromDash3, rays.toDash3, colors.dashCanceled)
end

function onCollision(with)
end
