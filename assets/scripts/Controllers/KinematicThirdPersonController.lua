function onStart()
    speed = 0
    strafe = 0
end

function onUpdate()
    camera = Components:Camera():getCamera()
    pos = this:getPosition()

    componentInput = Components:Input();
    local mouseMotionXRel = 0
    local clickLeft = componentInput:isLeftMouseButtonPressed()
    local isMouseMotion = componentInput:isMouseMotion()
    if (clickLeft and isMouseMotion) then
        mouseMotionXRel = componentInput:getMouseMotionXRel()
    end

    local yaw = (mouseMotionXRel * mouseSensibility);

    this:setRotation(this:getRotation() * M3:getMatrixRotationForEulerAngles(yaw, 0, 0))
    forward = this:getRotation():Z()
    camera:setPosition(pos + (forward:getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0)))
    camera:LookAt(this)
    if (Components:Input():isCharPressed("W")) then speed = speed - force end
    if (Components:Input():isCharPressed("S")) then speed = speed + force end
    if (Components:Input():isCharPressed("A")) then strafe = strafe - force end
    if (Components:Input():isCharPressed("D")) then strafe = strafe + force end

    if (Components:Input():isCharPressed("SPACE") and this:onGround()) then
        this:jump(Vertex3D.new(0, jumpForce, 0)) -- saltamos
    end

    velocity = forward:getScaled(speed) + this:getRotation():X():getScaled(strafe)
    if (this:onGround()) then
        this:setWalkingDirection(velocity) -- movemos
    else
        this:setWalkingDirection(velocity:getScaled(0.25)) -- movemos
    end

    speed = speed * deceleration;
    strafe = strafe * deceleration;
end

function onCollision(with)
end

