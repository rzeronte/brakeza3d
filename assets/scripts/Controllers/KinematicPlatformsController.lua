function onStart()
    speed = 0
end

function onUpdate()
    componentInput = componentsManager:getComponentInput();

    if (componentInput:isCharPressed("A")) then speed = speed + force end
    if (componentInput:isCharPressed("D")) then speed = speed - force end

    if (this:onGround()) then
        this:setWalkingDirection(Vertex3D.new(speed, 0, 0)) -- movemos
    else
        this:setWalkingDirection(Vertex3D.new(speed, 0, 0):getScaled(0.5)) -- movemos
    end

    if (componentInput:isCharPressed("SPACE") and this:onGround()) then
        this:jump(Vertex3D.new(0, jumpForce, 0)) -- saltamos
    end

    forward = this:AxisForward()

    camera = componentsManager:getComponentCamera():getCamera()
    camera:setPosition(this:getPosition() + forward:getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0))
    camera:lookAt(this)

    speed = speed * deceleration;
end

function onCollision(with)
end

