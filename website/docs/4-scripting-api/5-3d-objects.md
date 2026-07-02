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


## Mesh3DAnimation
---

An animated 3D model loaded from an FBX file containing skeletal animations.

```lua
man = ObjectFactory.Mesh3DAnimation(
    "../assets/animations/walking.fbx",                 -- file path
    Vertex3D.new(0, -10, 40)                            -- position
)
```

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