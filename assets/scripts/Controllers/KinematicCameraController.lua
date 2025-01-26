function onStart()
    speed = 0
    strafe = 0
end

function onUpdate()
    camera = componentsManager:getComponentCamera():getCamera()
    camera:setPosition(this:getPosition() + cameraOffset)
    camera:lookAt(this)

    componentInput = componentsManager:getComponentInput();
    local mouseMotionXRel = 0
    local mouseMotionYRel = 0;
    local clickLeft = componentInput:isLeftMouseButtonPressed()
    local isMouseMotion = componentInput:isMouseMotion()
    if (clickLeft and isMouseMotion) then
        mouseMotionXRel = componentInput:getMouseMotionXRel()
        mouseMotionYRel = componentInput:getMouseMotionYRel()
    end

    local yaw = (mouseMotionXRel * 0.20);
    local pitch =  (mouseMotionYRel * 0.20);
    local pitch = math.max(-89, math.min(89, pitch))

    r = M3:getMatrixRotationForEulerAngles(pitch, yaw, 0)

    this:setRotation(this:getRotation() * M3:getMatrixRotationForEulerAngles(pitch, yaw, 0))

    forward = this:AxisForward()
    right = this:AxisRight()

    if (componentsManager:getComponentInput():isCharPressed("W")) then speed = speed + force end
    if (componentsManager:getComponentInput():isCharPressed("S")) then speed = speed - force end
    if (componentsManager:getComponentInput():isCharPressed("A")) then strafe = strafe - force end
    if (componentsManager:getComponentInput():isCharPressed("D")) then strafe = strafe + force end

    if (componentsManager:getComponentInput():isCharPressed("SPACE") and this:onGround()) then
        this:jump(Vertex3D.new(0, -jumpForce, 0)) -- saltamos
    end

    if (this:onGround()) then
        velocity = forward:getScaled(speed) + right:getScaled(strafe)
        this:setWalkingDirection(velocity) -- movemos
    end

    speed = speed * deceleration;
    strafe = strafe * deceleration;
end

function onCollision(with)
end

