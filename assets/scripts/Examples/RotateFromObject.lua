function onStart()
    offset = Vertex3D.new(radius, 0, 0);
    objectName = "pawn"
end

function onUpdate()
    o = Brakeza:getObjectByName(objectName)

    center = o:getPosition()

    offset = M3:getMatrixRotationForEulerAngles(0, speed, 0) * offset;
    this:setPosition(center + offset);
end

function onCollision(with)
end
    