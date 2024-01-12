function onStart()
    offset = Vertex3D.new(0, radius, 0);
end

function onUpdate()
    offset = M3:getMatrixRotationForEulerAngles(0, speed, 0) * offset;
    this:setPosition(center + offset);
end
