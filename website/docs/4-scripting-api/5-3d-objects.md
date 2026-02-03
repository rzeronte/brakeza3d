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

## Drawing Lines
---

You can draw 3D lines using the Render component.

```lua
Components:Render():DrawLine(
    this:getPosition(),                                 -- from position
    this:getPosition() + this:AxisUp():getScaled(2)     -- to position
)
```