function onStart()
    eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
    eye:setEnabled(true)
    eye:setBelongToScene(false)
    eye:setScale(10)
    brakeza:addObject3D(eye, 'my_eye_model')
end

function onUpdate()
end

