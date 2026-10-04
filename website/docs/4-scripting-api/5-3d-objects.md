---
sidebar_position: 5
slug: 3d-objects
title: 3D Objects
description: 3D objects in Brakeza3D including Image3D, Mesh3D, Mesh3DAnimation, and line drawing.
---

# 3D objects
---

## Image3D
---

A plane with an image texture applied.

```lua
image3d = ObjectFactory.Image3D(
    "../assets/sprites/explosion_a.png",                    -- file path
    Vertex3D.new(10, 10, 10),                               -- position
    150,                                                    -- width
    200                                                     -- height
)
```

### Animated billboards (Image3DAnimation)

Animated sprite billboards are exposed to Lua as `BillboardAnimation`. Besides `CreateAnimation` and
`setAnimation`, they offer:

| Method | Parameters | Description |
|--------|------------|-------------|
| `setTowardsCamera()` | `bool` | When `true`, the quad always faces the camera. Set `false` for flat sprites lying on the ground (decals, puddles) |
| `setSize()` | `float w, float h` | Resizes the quad at runtime (e.g. a growing puddle effect) |
| `setRenderFB()` | `string fb` | Draws the billboard into a named framebuffer layer (`"scene"`, `"ui"`...) |

Objects found by name are returned as a plain `Object3D`. To get the full billboard API, use the
typed accessor:

```lua
local fx = Brakeza:getImage3DAnimationByName("explosion_01")
if fx then fx:setSize(4, 4) end
```


## Mesh3D
---

A 3D model that includes geometry and textures.

```lua
ObjectFactory.Mesh3D(
    "../assets/models/Capsule.fbx",                         -- file path
    Vertex3D.new(x, y, z)                                   -- position
)
```

Mesh3D objects provide some advanced features, such as Grid3D and Octrees, which will be covered in the
scripting chapter.

:::tip Shared geometry and instancing
Several `Mesh3D` / `Mesh3DAnimation` objects created from the **same model file** share their GPU
geometry, and the deferred renderer draws them with instancing (G-Buffer, shadow and picking passes,
animated models included). Spawning many copies of the same model is therefore much cheaper than
loading different files.
:::

### Mesh3D Methods

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `getModelFile()` | - | string | Path of the model file the object was created from (valid right after creation, before async loading ends) |
| `getMeshCount()` | - | int | Number of submeshes (one texture slot each) |
| `setDiffuseTexture()` | `int index, string path` | void | Replaces the diffuse texture of one submesh (e.g. uniforms or skins) |
| `getDiffuseTextureFile()` | `int index` | string | File name of the diffuse texture loaded in that submesh (`""` if none) |
| `setEmissionEnabled()` | `bool` | void | Turns emission on/off (off by default) |
| `isEmissionEnabled()` | - | bool | Returns whether emission is enabled |
| `setEmissionIntensity()` | `float 0..1` | void | How much the object glows with its own diffuse color, ignoring lighting |
| `getEmissionIntensity()` | - | float | Returns the emission intensity |

### Emission

An emissive mesh is drawn with its own diffuse color regardless of the scene lighting: screens, neon
signs, lamps, magic effects... Emission is **off by default**; enable it in the object inspector or from Lua:

```lua
local sign = Brakeza:getObjectByName("neon_sign")
sign:setEmissionEnabled(true)
sign:setEmissionIntensity(0.8)   -- 0 = lit normally, 1 = full self-illumination
```

Emissive objects are rendered in their own pass, so scenes without any emissive object pay no extra cost.


## Mesh3DAnimation
---

An animated 3D model loaded from an FBX file containing skeletal animations.

```lua
man = ObjectFactory.Mesh3DAnimation(
    "../assets/animations/walking.fbx",                 -- file path
    Vertex3D.new(0, -10, 40)                            -- position
)
```

Objects found by name are returned as a plain `Object3D`. Use `Brakeza:getMesh3DAnimationByName(name)`
to get the object with the full animation API.

### Mesh3DAnimation Methods

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `setIndexCurrentAnimation()` | `int index` | void | Changes the active animation by index |
| `setAnimationByName()` | `string name` | void | Changes the active animation by name |
| `setAnimationSpeed()` | `float speed` | void | Sets playback speed (0.0 - 1.0) |
| `isAnimationEnds()` | - | bool | Returns true if the current animation has finished |
| `setLoop()` | `bool loop` | void | Enables or disables animation looping |
| `isLoop()` | - | bool | Returns whether the animation loops |

### Change Current Animation

If the model contains more than one animation, the first one will be selected by default.
You can choose a different active animation from the GUI or via LUA scripts:

```lua
man:setIndexCurrentAnimation(2)                         -- change animation by index
man:setAnimationByName("Run")                           -- change animation by name
```

### Change Animation Speed

To modify the animation playback speed:

```lua
man:setAnimationSpeed(0.5)                              -- 50% speed
man:setAnimationSpeed(1.0)                              -- normal speed
```

### Animation Looping and Completion

```lua
man:setLoop(false)                                      -- disable looping

function onUpdate()
    if man:isAnimationEnds() then
        print("Animation finished!")
        man:setIndexCurrentAnimation(0)                 -- switch to idle
    end
end
```

## Image3DAnimation360
---

`Image3DAnimation360` (Lua type: `BillboardAnimation8Directions`) is a billboard sprite that
automatically selects one of **8 directional animation strips** based on the angle between the
camera and the object. It is the standard entity for top-down or isometric units that need to
appear to face the camera correctly from any viewing direction (e.g. soldiers, civilians, vehicles).

Each directional strip is a separate sprite sheet stored in a sub-folder. The object picks the
correct strip every frame using the horizontal angle between the camera forward vector and the
object's facing direction, divided into 8 equal sectors of 45°.

### Creating from Lua

```lua
local unit = ObjectFactory.Image3DAnimation360(
    "../assets/sprites/soldier/",   -- root folder (used by the serializer)
    Vertex3D.new(0, 0, 50),        -- world position
    2.0,                            -- billboard width  (world units)
    3.5                             -- billboard height (world units)
)
unit:setName("soldier_01")
Brakeza:AddObject3D(unit, "soldier_01")
```

### Adding directional animations

After creation, register each animation strip with `CreateAnimationDirectional2D`. The strips are
indexed internally in the order they are added:

```lua
-- The Lua binding for CreateAnimationDirectional2D is not currently exposed;
-- use the JSON scene format or the editor to configure directional animations.
```

:::note
`Image3DAnimation360` directional animations are typically configured in the scene JSON rather
than at runtime from Lua. The editor (Object Properties window) provides a GUI to add and preview
each of the 8 directional strips.
:::

### Lua methods

`BillboardAnimation8Directions` inherits all standard `Object3D` methods (position, rotation,
scale, enable/disable, collision, etc.). No additional Lua methods are exposed beyond those
inherited from `Object3D`.

| Inherited method | Description |
|---|---|
| `setPosition(Vertex3D)` | Move the billboard to a world position |
| `setEnabled(bool)` | Show or hide the object |
| `setRemoved(bool)` | Mark for deletion |
| `isRemoved()` | Returns true if the object has been removed |
| `setName(string)` | Set the object's name |
| `getPosition()` | Returns the current world position |

### Scene file format

```json
{
  "name": "soldier_01",
  "type": 11,
  "position": { "x": 0, "y": 0, "z": 50 },
  "width": 2.0,
  "height": 3.5,
  "animations": [
    { "file": "../assets/sprites/soldier/south.png",     "frames": 8, "fps": 12, "zeroDirection": true,  "maxTimes": 0 },
    { "file": "../assets/sprites/soldier/southwest.png", "frames": 8, "fps": 12, "zeroDirection": false, "maxTimes": 0 },
    { "file": "../assets/sprites/soldier/west.png",      "frames": 8, "fps": 12, "zeroDirection": false, "maxTimes": 0 },
    { "file": "../assets/sprites/soldier/northwest.png", "frames": 8, "fps": 12, "zeroDirection": false, "maxTimes": 0 },
    { "file": "../assets/sprites/soldier/north.png",     "frames": 8, "fps": 12, "zeroDirection": false, "maxTimes": 0 },
    { "file": "../assets/sprites/soldier/northeast.png", "frames": 8, "fps": 12, "zeroDirection": false, "maxTimes": 0 },
    { "file": "../assets/sprites/soldier/east.png",      "frames": 8, "fps": 12, "zeroDirection": false, "maxTimes": 0 },
    { "file": "../assets/sprites/soldier/southeast.png", "frames": 8, "fps": 12, "zeroDirection": false, "maxTimes": 0 }
  ]
}
```

`zeroDirection: true` marks the strip that represents the canonical facing direction (angle 0).
`maxTimes: 0` means the animation loops indefinitely.

---

## Common Object3D Methods
---

These methods are available on all 3D objects (Mesh3D, Mesh3DAnimation, Image3D, etc.).

### Orientation

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `LookAt()` | `Object3D target` | void | Rotates the object to face another object |
| `LookAt()` | `Vertex3D position` | void | Rotates the object to face a world position |
| `getModelMatrix()` | — | M3 | Returns the current 4×4 model matrix |

```lua
-- Look at another object
this:LookAt(enemy)

-- Look at a fixed position
this:LookAt(Vertex3D.new(0, 0, 100))
```

### Script Variables

Per-object variables stored in the object's Lua environment. Readable and writable from any script.

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `getLocalScriptVar()` | `string name` | any | Reads a variable from the object's script environment |
| `setLocalScriptVar()` | `string name, any value` | void | Writes a variable into the object's script environment |

```lua
-- Write from any script
unit:setLocalScriptVar("hp", 50)

-- Read from any script
local hp = tonumber(unit:getLocalScriptVar("hp")) or 0
```

### Selection & Highlight

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `isSelectable()` | — | bool | Returns whether the object can be picked by mouse click |
| `setSelectable()` | `bool value` | void | Enables or disables mouse-click selection for this object |
| `setHighlight()` | `float r, g, b, a` | void | Draws a colored outline around the object |
| `clearHighlight()` | — | void | Removes the outline |

```lua
-- Prevent terrain from being selected
terrain:setSelectable(false)

-- Highlight an enemy in red
enemy:setHighlight(1, 0, 0, 1)

-- Remove highlight
enemy:clearHighlight()
```

### Rendering Flags

| Method | Parameters | Return | Description |
|--------|------------|--------|-------------|
| `isAlphaEnabled()` | — | bool | Returns whether alpha blending is active |
| `setAlphaEnabled()` | `bool value` | void | Enables or disables alpha blending for this object |
| `isEnableLights()` | — | bool | Returns whether the object is affected by lights |
| `setEnableLights()` | `bool value` | void | Toggles lighting on this object |
| `isVisibleInFrustum()` | — | bool | Returns true if the object is currently inside the camera frustum |

```lua
-- Make an object unlit (e.g. a UI mesh or emissive prop)
prop:setEnableLights(false)

-- Transparent object
ghost:setAlphaEnabled(true)

-- Skip expensive logic for off-screen objects
if not unit:isVisibleInFrustum() then return end
```

## Drawing Lines
---

You can draw 3D lines using the Render component.

```lua
Components:Render():DrawLine(
    this:getPosition(),                                 -- from position
    this:getPosition() + this:AxisUp():getScaled(2)     -- to position
)
```