---
title: Your First Project
description: Step-by-step tutorial to create your first game in Brakeza3D with a rotating cube and keyboard controls.
sidebar_position: 3
---

# Your First Project
---

This tutorial will guide you through creating a simple interactive scene in **Brakeza3D**. By the end, you'll have a rotating 3D cube that responds to keyboard input.

## Prerequisites
---

- Brakeza3D installed and running
- Basic understanding of Lua programming

## Step 1: Create a New Project
---

1. Launch **Brakeza3D**
2. Go to `Brakeza` → `Documentation` to familiarize yourself with the interface
3. The engine starts with an empty scene by default

![Empty scene](/img/getting-started/sc01.png)

## Step 2: Add a 3D Object
---

Let's add a cube to the scene:

1. Go to **Add Object** → **Mesh3D**
2. Navigate to `assets/models/` and select a model (e.g., `Cube.fbx`)
3. The cube will appear at position (0, 0, 0)

Alternatively, you can use the **Models Browser** widget:
1. Open **Window** → **Models Browser**
2. Drag and drop a model into the scene

## Step 3: Create Your First Script
---

Now let's make the cube interactive with a Lua script.

### Create the Script File

1. Open **Window** → **Scripts Browser**
2. Right-click in the scripts area and select **New Script**
3. Name it `RotatingCube.lua`

### Write the Script Code

Double-click on `RotatingCube.lua` to open it in the **Code Editor**, then paste:

```lua
-- RotatingCube.lua
-- A simple script that rotates an object and responds to keyboard input

-- Variables
local rotationSpeed = 1.0
local moveSpeed = 5.0

function onStart()
    -- Called once when scripts start playing
    print("RotatingCube script started!")
end

function onUpdate()
    -- Called every frame
    local dt = Brakeza:getDeltaTime()

    -- Get the current rotation
    local rotation = this:getRotation()

    -- Create a rotation matrix for Y-axis rotation
    local angle = rotationSpeed * dt
    local newRotation = M3.getMatrixRotationForEulerAngle(0, angle, 0)

    -- Apply the rotation
    this:setRotation(rotation * newRotation)

    -- Handle keyboard input
    local input = Components:Input()
    local position = this:getPosition()

    if input:isCharPressed("w") then
        position.z = position.z + moveSpeed * dt
    end
    if input:isCharPressed("s") then
        position.z = position.z - moveSpeed * dt
    end
    if input:isCharPressed("a") then
        position.x = position.x - moveSpeed * dt
    end
    if input:isCharPressed("d") then
        position.x = position.x + moveSpeed * dt
    end

    this:setPosition(position)
end
```

### Save the Script

Press `Ctrl+S` or click the save button in the Code Editor.

## Step 4: Attach the Script to the Object
---

1. **Select the cube** by clicking on it in the scene or in the **Scene Objects** window
2. Open the **Object Scripts** window (Window → Object Scripts)
3. **Drag and drop** `RotatingCube.lua` from the Scripts Browser into the Object Scripts window

## Step 5: Run Your Script
---

1. Click the **Play** button in the toolbar (or go to **Script Controls** → **Play Scripts**)
2. Your cube should now be rotating!
3. Use **W, A, S, D** keys to move the cube around

To stop the scripts, click **Stop** in the toolbar.

## Step 6: Save Your Scene
---

1. Go to **Window** → **Scenes Browser**
2. Click the **Save** button
3. Name your scene (e.g., `MyFirstScene.json`)

## Step 7: Save Your Project
---

1. Go to **Window** → **Projects Browser**
2. Click **Save Project**
3. Name your project (e.g., `MyFirstProject.json`)

## Understanding the Code
---

Let's break down the key concepts:

### The `this` Variable

In object scripts, `this` refers to the object the script is attached to. You can:
- Get/set position: `this:getPosition()`, `this:setPosition(pos)`
- Get/set rotation: `this:getRotation()`, `this:setRotation(rot)`
- Get/set scale: `this:getScale()`, `this:setScale(scale)`

### Lifecycle Functions

| Function | When Called | Use Case |
|----------|-------------|----------|
| `onStart()` | Once when Play is pressed | Initialize variables, setup |
| `onUpdate()` | Every frame | Game logic, movement, input |
| `onCollision(with)` | When collision occurs | Collision response |

### Delta Time

`Brakeza:getDeltaTime()` returns the time since the last frame in seconds. **Always multiply movement by delta time** to ensure consistent speed regardless of frame rate.

### Components

Access engine systems through `Components:`:
- `Components:Input()` - Keyboard, mouse, gamepad
- `Components:Camera()` - Camera control
- `Components:Render()` - Rendering settings
- `Components:Sound()` - Audio playback
- `Components:Collisions()` - Physics system

## Next Steps
---

Congratulations! You've created your first Brakeza3D project. Here are some ideas to explore:

1. **Add lighting**: Try adding a `PointLight` or `SpotLight` to the scene
2. **Add collisions**: Enable physics on your cube with `setupGhostCollider()` or `SetupRigidBodyCollider()`
3. **Create a UI**: Use `Image2D` to display HUD elements
4. **Add sound**: Load and play audio with `Components:Sound()`

Continue to the [Scripting API](../scripting-api/index) section for a complete reference of all available functions.
