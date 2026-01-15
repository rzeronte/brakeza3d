---
sidebar_position: 3
title: 4.3) The Camera object
---

# Camera
---

**Brakeza3D** includes a specific object for the camera: **Camera3D**. After all, it is an `Object3D`, so it can be
moved and rotated like any other object.

It includes configuration options such as FOV and frustum sizes, which determine how and what is displayed
on screen.


## Object Camera3D
---

### Properties

| Property           | Type       | Description                     |
|--------------------|------------|---------------------------------|
| `pitch`            | `float`    | Rotation around the X-axis      |
| `yaw`              | `float`    | Rotation around the Y-axis      |
| `roll`             | `float`    | Rotation around the Z-axis      |
| `fov`              | `fov`      | Field of view                   |
| `velocity`         | `Vector3D` | Current velocity vector         |
| `viewMatrix`       | `M3`       | View matrix of the camera       |
| `projectionMatrix` | `M3`       | Projection matrix of the camera |

### Methods

| Method                      | Return Type | Description                                       |
|-----------------------------|-------------|---------------------------------------------------|
| `Pitch(float value)`        | `void`      | Adjusts the pitch (rotation around X-axis)        |
| `Yaw(float value)`          | `void`      | Adjusts the yaw (rotation around Y-axis)          |
| `MoveVertical(float value)` | `void`      | Moves the camera vertically                       |
| `MoveForward(float value)`  | `void`      | Moves the camera forward                          |
| `MoveBackward(float value)` | `void`      | Moves the camera backward                         |
| `StrafeRight()`             | `void`      | Moves the camera to the right                     |
| `StrafeLeft()`              | `void`      | Moves the camera to the left                      |
| `getYaw()`                  | `float`     | Returns a reference to the yaw value              |
| `getPitch()`                | `float`     | Returns a reference to the pitch value            |
| `getRoll()`                 | `float`     | Returns a reference to the roll value             |
| `getVelocity()`             | `Vector3D`  | Returns a reference to the camera velocity vector |
| `getM3ViewMatrix()`         | `M3`        | Returns the view matrix in `M3` format            |
| `getM3ProjectionMatrix()`   | `M3`        | Returns the projection matrix in `M3` format      |
| `setFOV(float v)`           | `void`      | Sets the field of view for all cameras            |

## View and Projection Matrices
---

Through the `Camera3D` object, we can obtain the view and projection matrices:

- `getM3ViewMatrix` : Returns an `M3` containing the view matrix data
- `getM3ProjectionMatrix` : Returns an `M3` containing the projection matrix data

These matrices are essential for rendering a 3D scene:

- The **view matrix** represents the camera's position and orientation in the scene, effectively transforming world coordinates into camera (or view) space.
- The **projection matrix** defines how the 3D scene is projected onto the 2D screen, including aspects like field of view, aspect ratio, and near/far clipping planes.

Together, these matrices allow **Brakeza3D** to render objects correctly from the camera's perspective, making them fundamental for both rendering and calculations like picking, culling, or raycasting.


## Camera movement and rotation
---

As an `Object3D`, the camera can be moved using the `setPosition(Vertex3D)` method and rotated using the `setRotation(M3)` method.  

In addition, Brakeza3D integrates a built-in system for camera movement and rotation using keyboard and mouse input.  
We will cover the details of this system later when discussing the **Input** component.


## Component Camera
---

**Brakeza3D** manages camera operations through the Camera component.

You can access this component from your Lua scripts, which gives you access to the camera object:

```lua
camera = Components:Camera():getCamera()
camera:setPosition(Vertex3D.new(10, 10, 10))
```


## FOV
---
You can adjust the horizontal FOV from your scripts as follows:

```lua
local camera = Components:Camera():getCamera()
camera:setFOV(140)
```