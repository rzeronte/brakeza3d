function onStart()
end

function onUpdate()
	o = brakeza:getSceneObjectByLabel("MyObject")
	if o ~= nil then
	    position = o:getLocalScriptVar("offset")
		print("Read variable 'offset' from object: ".. o:getLabel())
        print("Value for 'offset': " .. position.x .. ", " .. position.y .. ", " .. position.z)

		print("Read variable 'count' from object: ".. o:getLabel())
        print("Value for 'count': " .. o:getLocalScriptVar("count"))
    else
        print("Object not found")
	end
end