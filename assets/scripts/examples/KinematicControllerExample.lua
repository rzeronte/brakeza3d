function onStart()
    speed = Vertex3D.new(0, 0, 0);
end

function onUpdate()
    local force = 0.25

    if (componentsManager:getComponentInput():isCharPressed("W")) then
        speed = speed + Vertex3D.new(0, 0, force)
    end

    if (componentsManager:getComponentInput():isCharPressed("S")) then
        speed = speed + Vertex3D.new(0, 0, -force)
    end

    if (componentsManager:getComponentInput():isCharPressed("A")) then
        speed = speed + Vertex3D.new(-force, 0, 0)
    end

    if (componentsManager:getComponentInput():isCharPressed("D")) then
        speed = speed + Vertex3D.new(force, 0, 0)
    end

    if (componentsManager:getComponentInput():isCharPressed("SPACE")) then
        this:jump(Vertex3D.new(0, -20, 0)) -- saltamos
    end

    this:setWalkingDirection(speed) -- movemos

    speed = speed:getScaled(0.5); -- frenamos
end

function onCollision(with)
end