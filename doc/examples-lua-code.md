> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

## Ejemplos código LUA

- [Carga de objetos](#carga-de-objetos)
- [Carga de escenas](#carga-de-escenas)
- [Buscar un objeto](#buscar-un-objeto)
- [La varialbe this](#la-variable-this)
- [Variables globales](#variables-globales)
- [Variables de objeto](#variables-de-objeto)
- [Teclado y ratón](#teclado-y-ratón)
- [Detección de colisiones](#detección-de-colisiones)
- [Carga y reproducción de sonidos](#carga-y-reproducción-de-sonidos)
- [Escribir texto en pantalla](#escribir-texto-en-pantalla)

---

### Carga de objetos

A continuación el código LUA para cargar distintos tipos de objetos en Brakeza3D:

Image2D:
```lua
    img = Image2D.create(10, 10, "../assets/images/logo_small.png")
    img:updatePosition(300, 300)
    brakeza:addObject3D(img, 'myImage2D')
```

Image2DAnimation:
```lua
    animation2d = Image2DAnimation.create(100, 100, "../assets/sprites/explosion_a.png", 128, 128, 15, 24)
    brakeza:addObject3D(animation2d, 'myAnimation')
```

Image3D:
```lua
    image3d = Image3D.create(Vertex3D.new(10, 10, 10), 10, 10, "../assets/sprites/explosion_a.png")
    brakeza:addObject3D(image3d, 'myImage3D')
```

Mesh3D:
```lua
    eye = Mesh3D.create(Vertex3D.new(0, 0, 10), "../assets/models/eye.fbx")
    eye:setStencilBufferEnabled(true)
    eye:setBelongToScene(false)
    eye:setRotationFrameEnabled(true)
    eye:setRotationFrame(Vertex3D.new(1, 0, 0))
    eye:setScale(10)
    brakeza:addObject3D(eye, 'myMesh')
```
Mesh3DAnimation:
```lua
    man = Mesh3DAnimation.create(Vertex3D.new(0, -10, 40), "../assets/animations/walking.fbx")
    man:setStencilBufferEnabled(true)
    man:setBelongToScene(false)
    man:setScale(0.01)
    brakeza:addObject3D(man, 'myMeshAnimated')
```

BillboardAnimation:
```lua
    print("Load BillboardAnimation")
    billboard = BillboardAnimation.create(Vertex3D.new(0, 10, 80), 100, 100, "../assets/sprites/explosion_a.png", 128, 128, 15, 24)
    brakeza:addObject3D(billboard, 'myBillboard')
```

BillboardAnimation8Directions:
```lua
    print("Load BillboardAnimation8Directions")
    billboard8d = BillboardAnimation8Directions.create(Vertex3D.new(0, 10, 80), 100, 100, "../assets/sprites/Sprites3D/soldier/fire", 2, 24)
    brakeza:addObject3D(billboard8d, 'myBillboard8D')
```

---

### Carga de escenas

```lua
function onStart()
    ...
    componentsManager:getComponentRender():getSceneLoader():loadScene("../scenes/scene_example.json")
    ...
end
```

---

### Buscar un objeto

```lua
function onUpdate()
    ...
	o = brakeza:getSceneObjectByLabel("MyObject")
	if o ~= nil then
	    o:addToPosition(Vertex3D.new(-2, 0, 0))
	    position = o:getPosition()
	    local msg = "Item found, Position: " .. position.x .. ", " .. position.y .. ", " .. position.z
        print(msg)
    else
        print("Object not found")
	end
    ...
end
```

---

### La variable ``this``

```lua
function onUpdate()
    ...
    print("Hi I am " .. this:getLabel())
    ...
end
```

---

### Variables globales

```json
{
	"name":	"global_script_example.lua",
	"types": [
        {
          "name": "var1",
          "type": "string",
          "value": "hello my friend!"
        },
        {
          "name": "var2",
          "type": "int",
          "value": 10
        }      
    ]
}

```

```lua
function onUpdate()
    var1 = var1 .. "!"
    print("Value of var1: " .. var1)

    var2 = var2 + 1
    print("Value of var2: " .. var2)
end

```

---

### Variables de objeto

```json
{
	"name":	"object_variable.lua",
	"types": [
      {
        "name":	"count",
        "type":	"int",
        "value": 0
      },
      {
        "name":	"state",
        "type":	"int",
        "value": 0
      }
    ]
}

```

```lua
function onUpdate()
    scale = this:getScale()
    if scale > 1 and state == 1 then
        print("Increasing scale > 1 times: " .. count)
        state = 0
        count = count + 1
    end
    if scale <= 1 then
        state = 1
    end
end

```

```lua
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
```

---

### Teclado y ratón

```lua
function onUpdate()
    left = componentsManager:getComponentInput():isClickLeft()
    
    if (left)
        print("Click Left!")
    end
    
    if (componentsManager:getComponentInput():isCharPressed("A")) then
        print("Key A pressed!")
    end
    
    if (componentsManager:getComponentInput():isKeyEventDown()) then
        print("Key event down!")
    end
    
    if (componentsManager:getComponentInput():isKeyEventUp()) then
        print("Key event up!")
    end
end

```

---

### Detección de colisiones

```lua
function onCollision(with)
    if with ~= nil then
        print("Collision with " .. with:getLabel())
    else
        print("Collision with unknow object")
    end
end
````
---

### Carga y reproducción de sonidos

```lua
    systemSound = componentsManager:getComponentSound()
    systemSound:addSound("../assets/sounds/music_demo.mp3", "sound_demo")
    ...
    systemSound:playSound("sound_demo")
````

---

### Escribir texto en pantalla

```lua
function onStart()
    textWriter = TextWriter.create("../assets/fonts/Courier.ttf")
end

function onUpdate()
     textWriter:writeTTFCenterHorizontal(
        15,
        "Centrado horizontal!",
        Color.new(0, 1, 0, 1),
        1.5
    )

    textWriter:writeTextTTF(100, 100, 100, 100, "Hola!", Color.new(1, 0, 0, 1))
end

```
