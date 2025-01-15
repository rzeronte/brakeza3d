> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Ejemplo de carga de objetos

A continuación el código LUA para cargar distintos tipos de objetos en Brakeza3D:


```
function onStart()
    print("Load Image2D")
    img = Image2D.create(10, 10, "../assets/images/logo_small.png")
    img:updatePosition(300, 300)
    brakeza:addObject3D(img, 'myImage2D')

    print("Load Image2DAnimation")
    animation2d = Image2DAnimation.create(100, 100, "../assets/sprites/explosion_a.png", 128, 128, 15, 24)
    brakeza:addObject3D(animation2d, 'myAnimation')

    print("Load Image3D")
    image3d = Image3D.create(Vertex3D.new(10, 10, 10), 10, 10, "../assets/sprites/explosion_a.png")
    brakeza:addObject3D(image3d, 'myImage3D')

    print("Load Mesh3D")
    eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
    eye:setStencilBufferEnabled(true)
    eye:setBelongToScene(false)
    eye:setRotationFrameEnabled(true)
    eye:setRotationFrame(Vertex3D.new(1, 0, 0))
    eye:setScale(10)
    brakeza:addObject3D(eye, 'myMesh')

    print("Load Mesh3DAnimation")
    man = Mesh3DAnimation.create(Vertex3D.new(0, -10, 40), "../assets/animations/walking.fbx")
    man:setStencilBufferEnabled(true)
    man:setBelongToScene(false)
    man:setScale(0.01)
    brakeza:addObject3D(man, 'myMeshAnimated')

    print("Load BillboardAnimation")
    billboard = BillboardAnimation.create(Vertex3D.new(0, 10, 80), 100, 100, "../assets/sprites/explosion_a.png", 128, 128, 15, 24)
    brakeza:addObject3D(billboard, 'myBillboard')

    print("Load BillboardAnimation8Directions")
    billboard8d = BillboardAnimation8Directions.create(Vertex3D.new(0, 10, 80), 100, 100, "../assets/sprites/Sprites3D/soldier/fire", 2, 24)
    brakeza:addObject3D(billboard8d, 'myBillboard8D')

    print("Load Mesh3DGhost")
    meshghost = Mesh3DGhost.create(Vertex3D.new(10, 0, 10), "../assets/models/planet_cube_02.fbx")
    brakeza:addObject3D(meshghost, 'myMeshGhost')

    print("Load Mesh3DBody")
    meshbody = Mesh3DBody.create(Vertex3D.new(10, 10, 20), 1, "../assets/models/planet_cube_02.fbx")
    brakeza:addObject3D(meshbody, 'myMeshBody')
end

```
