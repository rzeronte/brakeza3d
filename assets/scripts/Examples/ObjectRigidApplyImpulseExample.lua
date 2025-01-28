function onStart()
end
function onUpdate()
    if (componentsManager:getComponentInput():isKeyEventDown()) then
        if (componentsManager:getComponentInput():isCharPressed("A")) then
            this:applyCentralImpulse(Vertex3D.new(0, -10, 0))
        end
    end

    if (componentsManager:getComponentInput():isKeyEventDown()) then
        if (componentsManager:getComponentInput():isCharPressed("S")) then
            this:applyCentralForce(Vertex3D.new(0, -10, 0))
        end
    end


    if (componentsManager:getComponentInput():isKeyEventDown()) then
        if (componentsManager:getComponentInput():isCharPressed("D")) then
            this:applyImpulse(Vertex3D.new(0, -10, 0), Vertex3D.new(0, 0, 0))
        end
    end
end

function onCollision(with)
end