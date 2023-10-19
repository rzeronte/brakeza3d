function onStart()
end

function onUpdate()
	o = brakeza:getSceneObjectByLabel(object)
	o:addToPosition(Vertex3D.new(-2, 0, 0))
end