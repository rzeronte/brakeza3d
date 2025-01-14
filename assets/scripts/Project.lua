function onStart()
    print("Comienzo Project.lua")
    img = Image2D.create(10, 10, "../assets/images/logo_small.png")
    img:setEnabled(true)
    img:updatePosition(300, 300)
    brakeza:addObject3D(img, 'imagen')

    eye = Mesh3D.create("../assets/models/eye.fbx")
    eye:setEnabled(true)
    eye:setStencilBufferEnabled(true)
    eye:setBelongToScene(false)
    eye:setRotationFrameEnabled(true)
    eye:setRotationFrame(Vertex3D.new(1, 0, 0))
    eye:setPosition(Vertex3D.new(0, 0, 10))
    eye:setScale(10)
    brakeza:addObject3D(eye, 'modelo')

    man = Mesh3DAnimation.create("../assets/animations/walking.fbx")
    man:setEnabled(true)
    man:setStencilBufferEnabled(true)
    man:setBelongToScene(false)
    man:setPosition(Vertex3D.new(0, -10, 10))
    man:setScale(0.25)
    brakeza:addObject3D(man, 'man')
end

function onUpdate()
end