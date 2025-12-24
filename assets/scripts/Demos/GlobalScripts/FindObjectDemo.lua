function onStart()
end

function onUpdate()
	o = Brakeza:getObjectByName("MyObject")
	if o ~= nil then
	    o:addToPosition(Vertex3D.new(-2, 0, 0))
	    position = o:getPosition()
	    local mensaje = "Item found, Position: " .. position.x .. ", " .. position.y .. ", " .. position.z
        print(mensaje)
    else
        print("Object not found")
	end
end