>[Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

- [Image3D](#image3d)
- [Mesh3D](#mesh3d)
- [Mesh3DAnimation](#mesh3danimation)
- [Mesh3DAnimationCollection](#mesh3danimationcollection)

---

## Image3D

Un plano con la textura de una imagen.

```lua
    -- Image3D.create(position, width, height, imageFile)
    image3d = Image3D.create(Vertex3D.new(10, 10, 10), 10, 10, "../assets/sprites/explosion_a.png")
    image3d:setEnabled(true)
    brakeza:addObject3D(image3d, 'my_image3d')
```
---

## Mesh3D
Modelo 3D, incorpora geometría y texturas.

```lua
    -- Mesh3D.create(position, modelFile)
    eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
    eye:setEnabled(true)
    eye:setStencilBufferEnabled(true)
    eye:setBelongToScene(false)
    eye:setRotationFrameEnabled(true)
    eye:setRotationFrame(Vertex3D.new(1, 0, 0))
    eye:setScale(10)
    brakeza:addObject3D(eye, 'modelo')
```
---

## Mesh3DAnimation

Modelo 3D animado.

```lua
    -- Mesh3DAnimation.create(position, animatedModelFile)
    man = Mesh3DAnimation.create(Vertex3D.new(0, -10, 40), "../assets/animations/walking.fbx")
    man:setEnabled(true)
    man:setStencilBufferEnabled(true)
    man:setBelongToScene(false)
    man:setScale(0.01)
    brakeza:addObject3D(man, 'myMeshAnimated')
```

---

## Mesh3DAnimationCollection

Agrupa un conjunto de animaciones de un modelo 3D.

---
