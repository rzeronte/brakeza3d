function onStart()
    speed = 0
    strafe = 0
    yaw = 0
    pitch = 0
end

function onUpdate()
    componentCamera = componentsManager:getComponentCamera();
    componentInput = componentsManager:getComponentInput();
    camera = componentCamera:getCamera()
    camera:setPosition(this:getPosition())

    local mouseMotionXRel = 0
    local mouseMotionYRel = 0;
    local clickLeft = componentInput:isLeftMouseButtonPressed()
    local isMouseMotion = componentInput:isMouseMotion()

    if (clickLeft and isMouseMotion) then
        mouseMotionXRel = componentInput:getMouseMotionXRel()
        mouseMotionYRel = componentInput:getMouseMotionYRel()
    end


    yaw = yaw + (mouseMotionXRel * 0.20);
    pitch = pitch + (mouseMotionYRel * 0.20);

    pitch = math.max(-89, math.min(89, pitch))

    camera:setRotationFromEulerAngles(pitch, -yaw, 0)


    this:setRotation(camera:getRotation():getTranspose())

    forward = this:AxisForward()
    right = this:AxisRight()

    if (componentsManager:getComponentInput():isCharPressed("W")) then
        speed = speed + force
    end

    if (componentsManager:getComponentInput():isCharPressed("S")) then
        speed = speed - force
    end

    if (componentsManager:getComponentInput():isCharPressed("A")) then
        strafe = strafe - force
    end

    if (componentsManager:getComponentInput():isCharPressed("D")) then
        strafe = strafe + force
    end

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