---
sidebar_position: 5
slug: 3d-objects
title: 4.5) 3D Objects
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

An animated 3D model.

```lua
man = ObjectFactory.Image3DAnimation(
    "../assets/animations/walking.fbx",
    Vertex3D.new(0, -10, 40),                           -- path file
    100,                                                -- width
    100,                                                -- height
    128,                                                -- sprite width
    128,                                                -- sprite height
    64,                                                 -- num frames
    25                                                  -- frames per second
);
```

### Change current animation

If the model contains more than one animation, the first one will be selected by default.
You can choose a different active animation from the GUI. To do so from LUA scripts, you can use
setIndexCurrentAnimation() or setAnimationByName():

```lua
man:setIndexCurrentAnimation(2)                         -- change animation index
```

### Change speed animation

To modify the animation playback speed, you can also do it from the UI or via scripts:

```lua
man:setAnimationSpeed(0.5)                              -- speed animation [0-1]
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