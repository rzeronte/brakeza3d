function onStart()
    speed = 0
    strafe = 0
end

function onUpdate()
    camera = componentsManager:getComponentCamera():getCamera()
    pos = this:getPosition()

    componentInput = componentsManager:getComponentInput();
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
    camera:lookAt(this)
    if (componentsManager:getComponentInput():isCharPressed("W")) then speed = speed - force end
    if (componentsManager:getComponentInput():isCharPressed("S")) then speed = speed + force end
    if (componentsManager:getComponentInput():isCharPressed("A")) then strafe = strafe - force end
    if (componentsManager:getComponentInput():isCharPressed("D")) then strafe = strafe + force end

    if (componentsManager:getComponentInput():isCharPressed("SPACE") and this:onGround()) then
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

