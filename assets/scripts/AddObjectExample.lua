function onStart()
    eye = Mesh3D:create()
    eye:setEnabled(true)
    eye:setStencilBufferEnabled(true)
    eye:setBelongToScene(false)
    eye:setRotationFrameEnabled(true)
    eye:setRotationFrame(Vertex3D.new(1, 0, 0))
    eye:setPosition(Vertex3D.new(0, 0, 6000))
    eye:setScale(1)
    eye:AssimpLoadGeometryFromFile("../assets/models/eye.fbx")

    brakeza:addObject3D(eye, brakeza:getExecutionTime())
end

function onUpdate()
end

