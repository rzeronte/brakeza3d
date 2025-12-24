function onStart()
end

function onUpdate()
	o = Brakeza:getObjectByName("MyObject")
	if o ~= nil then
	    position = o:getLocalScriptVar("offset")
		print("Read variable 'offset' from object: ".. o:getName())
        print("Value for 'offset': " .. position.x .. ", " .. position.y .. ", " .. position.z)

		print("Read variable 'count' from object: ".. o:getName())
        print("Value for 'count': " .. o:getLocalScriptVar("count"))
    else
        print("Object not found")
	end
end