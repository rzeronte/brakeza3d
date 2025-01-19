function onStart()
end

function onUpdate()
    dir = this:AxisForward()
    this:addToPosition(dir:getScaled(speed));
end