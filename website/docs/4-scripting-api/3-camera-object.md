---
sidebar_position: 3
title: The Camera object
description: Camera3D object in Brakeza3D with view and projection matrices, movement controls, FOV configuration, and practical examples.
---

# Camera
---

**Brakeza3D** includes a specific object for the camera: **Camera3D**. As an `Object3D`, it can be moved and rotated like any other object in the scene.

It includes configuration options such as FOV and frustum sizes, which determine how and what is displayed on screen.

## Accessing the Camera
---

Access the camera through the Camera component:

```lua
local camera = Components:Camera():getCamera()
```

## Camera3D Properties
---

| Property           | Type       | Description                     |
|--------------------|------------|---------------------------------|
| `pitch`            | `float`    | Rotation around the X-axis      |
| `yaw`              | `float`    | Rotation around the Y-axis      |
| `roll`             | `float`    | Rotation around the Z-axis      |
| `fov`              | `float`    | Field of view (degrees)         |
| `velocity`         | `Vector3D` | Current velocity vector         |
| `viewMatrix`       | `M3`       | View matrix of the camera       |
| `projectionMatrix` | `M3`       | Projection matrix of the camera |

## Camera3D Methods
---

### Position and Rotation

Since Camera3D inherits from Object3D, you can use all standard transformation methods:

```lua
camera:setPosition(Vertex3D.new(0, 10, -20))
camera:getPosition()
camera:setRotation(rotationMatrix)
camera:getRotation()
```

### setRotationFromEulerAngles

Sets the camera rotation using Euler angles (pitch, yaw, roll). This is the recommended way to control camera orientation.

```lua
camera:setRotationFromEulerAngles(pitch, yaw, roll)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pitch` | `float` | X-axis rotation (look up/down) in degrees |
| `yaw` | `float` | Y-axis rotation (look left/right) in degrees |
| `roll` | `float` | Z-axis rotation (tilt) in degrees |

```lua
-- Look slightly down and to the right
camera:setRotationFromEulerAngles(-15, 45, 0)
```

:::tip
Pitch is typically limited to -89 to 89 degrees to prevent gimbal lock issues.
:::

---

### setFOV

Sets the field of view (horizontal angle in degrees).

```lua
camera:setFOV(fov)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `fov` | `float` | Field of view in degrees (default: 90) |

```lua
-- Wide angle view
camera:setFOV(120)

-- Zoomed/sniper view
camera:setFOV(30)
```

---

### LookAt

Rotates the camera to face a target object.

```lua
camera:LookAt(targetObject)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `targetObject` | `Object3D` | The object to look at |

```lua
local player = Brakeza:getObjectByName("Player")
camera:LookAt(player)
```

---

### Movement Methods

| Method | Parameters | Description |
|--------|------------|-------------|
| `Pitch(value)` | `float` | Adjusts the pitch (rotation around X-axis) |
| `Yaw(value)` | `float` | Adjusts the yaw (rotation around Y-axis) |
| `MoveVertical(value)` | `float` | Moves the camera vertically |
| `MoveForward(value)` | `float` | Moves the camera forward |
| `MoveBackward(value)` | `float` | Moves the camera backward |
| `StrafeRight()` | - | Moves the camera to the right |
| `StrafeLeft()` | - | Moves the camera to the left |

---

### Getters

| Method | Return Type | Description |
|--------|-------------|-------------|
| `getYaw()` | `float` | Returns the yaw value |
| `getPitch()` | `float` | Returns the pitch value |
| `getRoll()` | `float` | Returns the roll value |
| `getVelocity()` | `Vector3D` | Returns the camera velocity vector |
| `getM3ViewMatrix()` | `M3` | Returns the view matrix |
| `getM3ProjectionMatrix()` | `M3` | Returns the projection matrix |

## View and Projection Matrices
---

The camera provides access to essential rendering matrices:

- **View Matrix** (`getM3ViewMatrix()`): Transforms world coordinates into camera space
- **Projection Matrix** (`getM3ProjectionMatrix()`): Projects 3D scene onto 2D screen

These matrices are fundamental for:
- Rendering objects from the camera's perspective
- Raycasting and picking
- Frustum culling
- Custom shader effects

```lua
local viewMatrix = camera:getM3ViewMatrix()
local projMatrix = camera:getM3ProjectionMatrix()
```

## Built-in Camera Controls
---

Brakeza3D includes a built-in system for camera movement using keyboard and mouse. Control it through the Input component:

```lua
function onStart()
    local input = Components:Input()

    -- Enable/disable mouse look
    input:setMouseEnabled(true)

    -- Enable/disable keyboard movement (WASD)
    input:setKeyboardEnabled(true)

    -- Enable/disable gamepad control
    input:setPadEnabled(false)
end
```

## Practical Examples
---

### First-Person Camera

Attach the camera to a player object with mouse look control:

```lua
-- Variables
local yaw = 0
local pitch = 0
local mouseSensibility = 0.3

function onStart()
    -- Disable built-in camera controls
    Components:Input():setMouseEnabled(false)
    Components:Input():setKeyboardEnabled(false)
end

function onUpdate()
    local camera = Components:Camera():getCamera()
    local input = Components:Input()

    -- Sync camera position with player (this)
    camera:setPosition(this:getPosition())

    -- Mouse look (only when left button pressed)
    if input:isLeftMouseButtonPressed() and input:isMouseMotion() then
        local mouseX = input:getMouseMotionXRel()
        local mouseY = input:getMouseMotionYRel()

        yaw = yaw + (mouseX * mouseSensibility)
        pitch = pitch + (mouseY * mouseSensibility)

        -- Clamp pitch to prevent flipping
        pitch = math.max(-89, math.min(89, pitch))
    end

    -- Apply rotation
    camera:setRotationFromEulerAngles(pitch, yaw, 0)

    -- Sync player rotation with camera (for movement direction)
    this:setRotation(camera:getRotation():getTranspose())
end
```

---

### Third-Person Camera

Camera follows behind the player:

```lua
-- Variables (configure in JSON)
local cameraOffset = Vertex3D.new(0, 5, 15)  -- x, y (height), z (distance)
local mouseSensibility = 0.5

function onUpdate()
    local camera = Components:Camera():getCamera()
    local input = Components:Input()
    local playerPos = this:getPosition()

    -- Rotate player with mouse
    if input:isLeftMouseButtonPressed() and input:isMouseMotion() then
        local mouseX = input:getMouseMotionXRel()
        local yawDelta = mouseX * mouseSensibility

        local newRotation = this:getRotation() * M3:getMatrixRotationForEulerAngles(yawDelta, 0, 0)
        this:setRotation(newRotation)
    end

    -- Calculate camera position behind player
    local forward = this:getRotation():Z()
    local cameraPos = playerPos + forward:getScaled(cameraOffset.z) + Vertex3D.new(cameraOffset.x, cameraOffset.y, 0)

    camera:setPosition(cameraPos)
    camera:LookAt(this)
end
```

---

### Cinematic Camera

Smooth camera movement between points:

```lua
local cameraPoints = {}
local currentPoint = 1
local t = 0
local speed = 0.5

function onStart()
    -- Define camera path
    cameraPoints = {
        {pos = Vertex3D.new(0, 10, -30), target = Vertex3D.new(0, 0, 0)},
        {pos = Vertex3D.new(20, 15, 0), target = Vertex3D.new(0, 5, 0)},
        {pos = Vertex3D.new(0, 20, 30), target = Vertex3D.new(0, 0, 0)},
    }
end

function onUpdate()
    local camera = Components:Camera():getCamera()
    local dt = Brakeza:getDeltaTime()

    -- Interpolate between points
    t = t + dt * speed

    if t >= 1 then
        t = 0
        currentPoint = currentPoint + 1
        if currentPoint > #cameraPoints then
            currentPoint = 1
        end
    end

    local nextPoint = currentPoint + 1
    if nextPoint > #cameraPoints then nextPoint = 1 end

    local from = cameraPoints[currentPoint]
    local to = cameraPoints[nextPoint]

    -- Smooth interpolation (lerp)
    local newPos = Vertex3D.new(
        from.pos.x + (to.pos.x - from.pos.x) * t,
        from.pos.y + (to.pos.y - from.pos.y) * t,
        from.pos.z + (to.pos.z - from.pos.z) * t
    )

    camera:setPosition(newPos)

    -- Look at interpolated target
    local targetObj = Brakeza:getObjectByName("CameraTarget")
    if targetObj then
        camera:LookAt(targetObj)
    end
end
```

---

### FOV Zoom Effect

Smooth zoom for aiming/scope:

```lua
local normalFOV = 90
local zoomFOV = 30
local currentFOV = normalFOV
local isZooming = false

function onUpdate()
    local camera = Components:Camera():getCamera()
    local input = Components:Input()
    local dt = Brakeza:getDeltaTime()

    -- Toggle zoom with right mouse button
    isZooming = input:isRightMouseButtonPressed()

    -- Smooth FOV transition
    local targetFOV = isZooming and zoomFOV or normalFOV
    local zoomSpeed = 200  -- degrees per second

    if currentFOV < targetFOV then
        currentFOV = math.min(currentFOV + zoomSpeed * dt, targetFOV)
    elseif currentFOV > targetFOV then
        currentFOV = math.max(currentFOV - zoomSpeed * dt, targetFOV)
    end

    camera:setFOV(currentFOV)
end
```

## Component Camera
---

The Camera component provides additional control:

| Method | Description |
|--------|-------------|
| `getCamera()` | Returns the Camera3D object |
| `setEnabled(bool)` | Enable/disable the camera component |
| `isEnabled()` | Check if camera component is enabled |
| `getGLMMat4ViewMatrix()` | Get view matrix as glm::mat4 |
| `getGLMMat4ProjectionMatrix()` | Get projection matrix as glm::mat4 |

```lua
-- Access component
local camComponent = Components:Camera()

-- Enable camera
camComponent:setEnabled(true)

-- Get camera object
local camera = camComponent:getCamera()
```

## Tips and Best Practices
---

1. **Clamp pitch**: Always limit pitch between -89 and 89 degrees to avoid gimbal lock
   ```lua
   pitch = math.max(-89, math.min(89, pitch))
   ```

2. **Use deltaTime**: Multiply movement and rotation by `Brakeza:getDeltaTime()` for consistent speed

3. **Disable built-in controls** when implementing custom camera:
   ```lua
   Components:Input():setMouseEnabled(false)
   Components:Input():setKeyboardEnabled(false)
   ```

4. **Smooth transitions**: Use interpolation (lerp) for smooth camera movements instead of instant jumps

5. **Camera shake**: Add small random offsets to position for impact effects:
   ```lua
   local shake = Vertex3D.new(
       math.random() * shakeIntensity - shakeIntensity/2,
       math.random() * shakeIntensity - shakeIntensity/2,
       0
   )
   camera:setPosition(basePosition + shake)
   ```
