function onStart()
end

function onUpdate()
    tiempo = brakeza:getExecutionTime()
    this:addToPosition(Vertex3D.new(0, math.sin(tiempo) * speed, 0))
end