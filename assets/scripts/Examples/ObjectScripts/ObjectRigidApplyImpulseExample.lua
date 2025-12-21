function onStart()
end

function onUpdate()
    if (Components:Input():isKeyEventDown()) then
        if (Components:Input():isCharPressed("A")) then
            this:applyCentralImpulse(Vertex3D.new(0, -10, 0))
        end
    end

    if (Components:Input():isKeyEventDown()) then
        if (Components:Input():isCharPressed("S")) then
            this:applyCentralForce(Vertex3D.new(0, -10, 0))
        end
    end


    if (Components:Input():isKeyEventDown()) then
        if (Components:Input():isCharPressed("D")) then
            this:applyImpulse(Vertex3D.new(0, -10, 0), Vertex3D.new(0, 0, 0))
        end
    end
end

function onCollision(with)
end