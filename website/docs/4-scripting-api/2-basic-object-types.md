---
sidebar_position: 2
title: Basic object types
description: Basic object types in Brakeza3D including Object3D, Vertex3D, M3 matrices, AABB, Point2D, and Color.
---

# Basic object types
---

## The Object3D object 
---

The most basic object in **Brakeza3D** is `Object3D`. Any element in the scene is an Object3D.


### Common properties

| Property   | Type       | Description                                  |
|------------|------------|----------------------------------------------|
| `Name`     | `string`   | Name of object                               |
| `Position` | `Vertex3D` | Position in the world                        |
| `Rotation` | `M3`       | Object rotation                              |
| `Scale`    | `float`    | Model scale factor                           |
| `Enabled`  | `bool`     | Determines if the object is active or not    |
| `Removed`  | `bool`     | Determines if the object is marked to remove |


## Interacting with an Object3D
---


We can interact with an Object3D using several methods:

#### Object3D API

---

### Getters

| Method               | Return       | Description                    |
|----------------------|--------------|--------------------------------|
| `getName()`          | `string`     | Gets the object's name         |
| `getPosition()`      | `Vertex3D`   | Gets the object's position     |
| `getRotation()`      | `M3`         | Gets the rotation              |
| `getScale()`         | `float`      | Gets the scale factor          |
| `getTypeObject()`    | `ObjectType` | Returns the object type        |
| `getM3ModelMatrix()` | `M3`         | Gets the object's model matrix |
| `getAlpha()`         | `float`      | Gets the object's opacity      |
| `AxisForward()`      | `Vertex3D`   | Returns the forward axis       |
| `AxisUp()`           | `Vertex3D`   | Returns the up axis            |
| `AxisRight()`        | `Vertex3D`   | Returns the right axis         |

---

### Setters

| Method                            | Return | Description                          |
|-----------------------------------|--------|--------------------------------------|
| `setName(name: string)`           | void   | Sets a new object name               |
| `setPosition(position: Vertex3D)` | void   | Sets a new position                  |
| `setRotation(rotation: M3)`       | void   | Sets a new rotation                  |
| `setScale(scale: float)`          | void   | Sets a new scale factor              |
| `setAlpha(alpha: float)`          | void   | Sets a new opacity factor            |
| `setEnable(enable: bool)`         | void   | Activates or deactivates the object  |
| `setRemoved(removed: bool)`       | void   | Removes the object in the next frame |

---

### Actions

| Method     | Parameters         | Description                               |
|------------|--------------------|-------------------------------------------|
| `LookAt()` | `target: Object3D` | Rotates the object to face another object |

It is also possible to add new Object3D instances to the scene using code via ObjectFactory.



## Scripting on Object3D
---


As we will see later, objects in **Brakeza3D** can be manipulated through scripts. Scripts allow you to encapsulate reusable logic and apply it across multiple objects.  

Multiple scripts can be attached to a single object, and objects can also be controlled from generic or scene-level scripts.



## Derived objects from Object3D
---

**Brakeza3D** includes several ready-to-use 3D objects, all derived from Object3D. Since they inherit from `Object3D`, these objects can also use all the methods and properties previously described for `Object3D`, providing consistency and flexibility when manipulating them in the scene.


| Object             | Description                                   |
|--------------------|-----------------------------------------------|
| `Mesh3D`           | 3D model with triangles and textures          |
| `Mesh3DAnimation`  | Similar to `Mesh3D`, but for animated models  |
| `Image2D`          | Draws an image on screen                      |
| `Image3D`          | 3D plane with an image texture                |
| `Image2DAnimation` | Set of `Image2D` objects forming an animation |
| `ParticleEmitter`  | Particle emitter                              |
| `LightPoint`       | Point light                                   |
| `LightSpot`        | Spotlight                                     |

Some of these objects will be detailed later in the documentation.

### Object factory
---

Additionally, Brakeza3D provides an `ObjectFactory` that allows developers to create almost any object in the 3D world programmatically, making scene construction highly flexible and dynamic.


## Auxiliary Basic Types
---

There are several data types that facilitate 3D tasks and are available in your Lua scripts.



### Vertex3D


Represents a **point in three-dimensional space** or a **vector originating from (0, 0, 0)**.  
It is used for geometric and vector algebra operations in 3D environments.

`Vertex3D` provides support for 3D coordinates and common vector operations such as addition, subtraction, vector products, normalization, scaling, and distance calculation.

#### Properties

| Property | Type    | Description  |
|----------|---------|--------------|
| `x`      | `float` | X coordinate |
| `y`      | `float` | Y coordinate |
| `z`      | `float` | Z coordinate |

#### Methods

| Method                    | Return Type | Description                                               |
|---------------------------|-------------|-----------------------------------------------------------|
| `operator +(Vertex3D)`    | `Vertex3D`  | Vector addition                                           |
| `operator -(Vertex3D)`    | `Vertex3D`  | Vector subtraction                                        |
| `operator *(Vertex3D)`    | `float`     | Cross product *(as defined by the project specification)* |
| `operator %(Vertex3D)`    | `Vertex3D`  | Dot product *(as defined by the project specification)*   |
| `getNormalize()`          | `Vertex3D`  | Returns the normalized vector                             |
| `getModule()`             | `float`     | Vector length (magnitude)                                 |
| `getInverse()`            | `Vertex3D`  | Returns the inverted vector                               |
| `getScaled(float factor)` | `Vertex3D`  | Returns the vector scaled by a factor                     |
| `distance(Vertex3D)`      | `float`     | Distance to another vertex                                |


### M3 (Matrix 3x3)

Represents a **3x3 floating-point matrix** used to define **rotations in 3D space**.  
It is commonly used for orientation, axis transformations, and rotating vectors.

`M3` encapsulates a 3x3 rotation matrix and provides basic matrix operations, axis extraction, and utilities for creating identity, transposed, and Euler-based rotation matrices.


#### Properties

| Property | Type       | Description                  |
|----------|------------|------------------------------|
| `values` | `float[9]` | Stores the 3x3 matrix values |

#### Methods

| Method                                    | Return Type | Description                                                    |
|-------------------------------------------|-------------|----------------------------------------------------------------|
| `operator +(M3)`                          | `M3`        | Matrix addition                                                |
| `operator -(M3)`                          | `M3`        | Matrix subtraction                                             |
| `operator *(M3)`                          | `M3`        | Matrix multiplication                                          |
| `operator *(Vertex3D)`                    | `Vertex3D`  | Multiplies the matrix by a vector                              |
| `X()`                                     | `Vertex3D`  | Returns the X axis vector                                      |
| `Y()`                                     | `Vertex3D`  | Returns the Y axis vector                                      |
| `Z()`                                     | `Vertex3D`  | Returns the Z axis vector                                      |
| `getMatrixIdentity()`                     | `M3`        | Returns the identity matrix                                    |
| `getTranspose()`                          | `M3`        | Returns the transposed matrix                                  |
| `getMatrixRotationForEulerAngle(p, y, r)` | `M3`        | Creates a rotation matrix from Euler angles (pitch, yaw, roll) |



### AABB (Axis-Aligned Bounding Box)

Represents an **Axis-Aligned Bounding Box (AABB)** in 3D space.  
An AABB is a bounding volume whose faces are aligned with the coordinate axes, commonly used for **collision detection** and **spatial queries**.

`AABB3D` defines a box using two vertices: the minimum and maximum corners.  
Because it is aligned with the coordinate axes, intersection and containment tests are fast and efficient.

#### Properties

| Property | Type       | Description                        |
|----------|------------|------------------------------------|
| `min`    | `Vertex3D` | Minimum corner of the bounding box |
| `max`    | `Vertex3D` | Maximum corner of the bounding box |

#### Methods

| Method                    | Return Type | Description                                       |
|---------------------------|-------------|---------------------------------------------------|
| `isColliding(AABB3D)`     | `bool`      | Checks overlap with another AABB                  |
| `size()`                  | `Vertex3D`  | Returns the size of the bounding box              |
| `getCenter()`             | `Vertex3D`  | Returns the center point of the bounding box      |
| `isPointInside(Vertex3D)` | `bool`      | Checks whether a point is inside the bounding box |


### Point2D

Represents a **point or vector in two-dimensional space** using integer coordinates.  
It is commonly used for grid-based logic, screen positioning, and 2D calculations.

`Point2D` provides basic arithmetic and utility operations for working with 2D vectors, including addition, subtraction, normalization, and scaling.

#### Properties

| Property | Type  | Description  |
|----------|-------|--------------|
| `x`      | `int` | X coordinate |
| `y`      | `int` | Y coordinate |

#### Methods

| Method                    | Return Type | Description                           |
|---------------------------|-------------|---------------------------------------|
| `operator +(Point2D)`     | `Point2D`   | Point/vector addition                 |
| `operator -(Point2D)`     | `Point2D`   | Point/vector subtraction              |
| `operator /(Point2D)`     | `Point2D`   | Component-wise division               |
| `getNormalize()`          | `Point2D`   | Returns the normalized vector         |
| `getScaled(float factor)` | `Point2D`   | Returns the vector scaled by a factor |


### Color

Represents an **RGBA color** with floating-point components.  
It is commonly used for rendering, shading, and color manipulation in 2D and 3D engines.

`Color` encapsulates red, green, blue, and alpha components, providing arithmetic operations, comparisons, and utility functions.  
Several **predefined colors** are available for convenience.

#### Constructors

| Constructor                                     | Description                                            |
|-------------------------------------------------|--------------------------------------------------------|
| `Color()`                                       | Default constructor, initializes to black (0, 0, 0, 0) |
| `Color(float r, float g, float b, float a = 0)` | Initializes color with specified RGBA values           |

#### Properties

| Property | Type    | Description                           |
|----------|---------|---------------------------------------|
| `r`      | `float` | Red component (0.0 – 1.0)             |
| `g`      | `float` | Green component (0.0 – 1.0)           |
| `b`      | `float` | Blue component (0.0 – 1.0)            |
| `a`      | `float` | Alpha (opacity) component (0.0 – 1.0) |

#### Methods

| Method               | Return Type | Description                          |
|----------------------|-------------|--------------------------------------|
| `operator +(Color)`  | `Color`     | Adds two colors component-wise       |
| `operator -(Color)`  | `Color`     | Subtracts two colors component-wise  |
| `operator *(Color)`  | `Color`     | Multiplies two colors component-wise |
| `operator *(float)`  | `Color`     | Multiplies color by a scalar         |
| `operator /(float)`  | `Color`     | Divides color by a scalar            |
| `operator ==(Color)` | `bool`      | Checks equality of two colors        |
| `setRed(float)`      | `void`      | Sets the red component               |
| `setGreen(float)`    | `void`      | Sets the green component             |
| `setBlue(float)`     | `void`      | Sets the blue component              |

#### Predefined Colors

| Name           | RGBA                  |
|----------------|-----------------------|
| `white()`      | (1, 1, 1, 0)          |
| `red()`        | (1, 0, 0, 0)          |
| `green()`      | (0, 1, 0, 0)          |
| `blue()`       | (0, 0, 1, 0)          |
| `black()`      | (0, 0, 0, 0)          |
| `yellow()`     | (1, 1, 0, 0)          |
| `fuchsia()`    | (1, 0, 1, 0)          |
| `cyan()`       | (0, 1, 1, 0)          |
| `orange()`     | (1, 0.5, 0.2, 0)      |
| `olive()`      | (0.5, 0.5, 0, 0)      |
| `gray()`       | (0.5, 0.5, 0.5, 0)    |
| `pink()`       | (1, 0.41, 0.71, 0)    |
| `lime()`       | (0.75, 1, 0, 0)       |
| `turquoise()`  | (0.25, 0.88, 0.82, 0) |
| `violet()`     | (0.93, 0.51, 0.93, 0) |
| `gold()`       | (1, 0.84, 0, 0)       |
| `indigo()`     | (0.29, 0, 0.51, 0)    |
| `magenta()`    | (1, 0, 0.56, 0)       |
| `aquamarine()` | (0.5, 1, 0.83, 0)     |
| `crimson()`    | (0.86, 0.08, 0.24, 0) |
| `FOGDefault()` | (0.5, 0.5, 0.5, 0)    |

### Image

Represents a **2D image object** in Brakeza3D that can be positioned, resized, and rendered on the screen.  
`Image2D` inherits from `Object3D`, allowing it to integrate seamlessly with the engine’s scene system while providing 2D-specific functionality.

#### Constructors

| Constructor                                       | Description                                                       |
|---------------------------------------------------|-------------------------------------------------------------------|
| `Image2D()`                                       | Default constructor                                               |
| `Image2D(string pathFile, int width, int height)` | Constructor that loads an image from file with the specified size |

#### Properties

| Property   | Type     | Description                   |
|------------|----------|-------------------------------|
| `x`        | `int`    | X position on the screen      |
| `y`        | `int`    | Y position on the screen      |
| `width`    | `int`    | Width of the image on screen  |
| `height`   | `int`    | Height of the image on screen |
| `filepath` | `string` | Path to the image file        |

#### Methods

| Method                                     | Return Type      | Description                                  |
|--------------------------------------------|------------------|----------------------------------------------|
| `setSize(int x, int y)`                    | `void`           | Sets the width and height of the image       |
| `setScreenPosition(int x, int y)`          | `void`           | Sets the X and Y screen position             |
| `setFilePath(const std::string &filepath)` | `void`           | Sets the file path for the image             |
| `getTypeObject()`                          | `ObjectType`     | Returns the type of object (`Image2D`)       |
