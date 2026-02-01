---
sidebar_position: 8
title: Lighting system
---

# Lighting system


It is configured via the render component:

```lua
render = Components:Render()
```

## Global illumination
---

**Brakeza3D** includes a default light in the scene, which can be thought of as the sun.

Essentially, it is a directional light, so you can adjust some of its parameters to achieve the desired
base illumination. Being a global light, it has no position. You can manipulate the components of global
illumination through the UI or using your LUA scripts.

### ADS Components

The ADS (*Ambient*, *Diffuse*, *Specular*) model is a standard lighting technique used in computer graphics to simulate how light interacts with objects in a 3D scene. It separates illumination into three components, allowing realistic control of ambient light, direct light reflection, and surface highlights in a simple and efficient way.

In **Brakeza3D**, the ADS lighting components are configured through specific methods that allow precise control over how an object reacts to light. Each method corresponds to one of the ADS components, enabling developers to define the intensity and color of ambient light, the influence of direct light on textures, and the strength of specular highlights on object surfaces.

| Component | Type | Description                                               |
|-----------|------|-----------------------------------------------------------|
| Ambient   | vec3 | Color intensity of the ambient light affecting the object |
| Diffuse   | vec3 | Color intensity of the light applied to the texture       |
| Specular  | vec3 | Color intensity of specular reflections                   |

These attributes can be modified from the GUI or via LUA scripts.
From scripts, you can use the following methods:

| Method                  | Description                                                                   |
|-------------------------|-------------------------------------------------------------------------------|
| setAmbient(Color)       | Sets the color intensity of the ambient light affecting the object            |
| setColor(Color)         | Sets the color intensity of the diffuse light applied to the object’s surface |
| setColorSpecular(Color) | Sets the color intensity of the specular reflections on the object            |

Methods form global illumination:

| Method                                  | Description                                            |
|-----------------------------------------|--------------------------------------------------------|
| setGlobalIlluminationAmbient(Vertex3D)  | Sets the ambient component of the global illumination  |
| setGlobalIlluminationDiffuse(Vertex3D)  | Sets the diffuse component of the global illumination  |
| setGlobalIlluminationSpecular(Vertex3D) | Sets the specular component of the global illumination |

:::note
The ambient, diffuse, and specular components are controlled via Vertex3D rather than a Color type.
This is for convenience, since the expected values range from 0 to 1, rather than 0 to 255 as with a Color.
:::

Example: Setting the Ambient Component

```lua
Components:Render():setGlobalIlluminationAmbient(Vertex3D.new(1, 0, 0)) -- rojo
```

### Direction

    - `setGlobalIlluminationDirection(Vertex3D)`: Sets the global illumination direction.

## Light objects
---

In addition to global lighting, **Brakeza3D** offers a variety of light objects that allow you to illuminate your scenes dynamically and with precision. These objects simulate realistic lighting effects, from ambient and diffuse illumination to spotlights and directional lights, giving you full control over the atmosphere and mood of your 3D environment. By using these light objects, you can enhance depth, highlight important elements, and create visually compelling scenes, all while maintaining efficient performance suitable for real-time applications.


### LightPoint
---

The `LightPoint` object represents a point of light in space. It inherits all the characteristics of `Object3D` and incorporates the basic properties needed to configure a light source that emits in all directions.

#### Attenuation Constants

Attenuation constants control how light intensity decreases with distance. The constant factor maintains a base intensity, the linear factor reduces light proportionally, and the quadratic factor causes a faster falloff, creating more realistic lighting. These parameters can be adjusted from the GUI or via LUA scripts.

| Parameter | Type  | Description                              | Method (LUA)        |
|-----------|-------|------------------------------------------|---------------------|
| Constant  | float | Maintains a base light intensity         | setConstant(float)  |
| Linear    | float | Reduces light proportionally to distance | setLinear(float)    |
| Quadratic | float | Causes faster falloff with distance      | setQuadratic(float) |


```lua
point = ObjectFactory.LightPoint(
    Vertex3D.new(10, 20, 30),                               -- position
    Color.new(0.1, 0.1, 0.1),                               -- ambient component
    Color.new(0.2, 0.4, 0.6),                               -- diffuse component
    Color.new(0.5, 0.3, 1.0),                               -- specular component
);
```

### LightSpot
---

A `LightSpot` is a light source that emits light in a specific direction, essentially functioning as a spotlight.

The `LightSpot` object directly inherits from `LightPoint`, so it includes all of its properties. In addition, it introduces the following specific attributes:

| Attribute | Type     | Description                                                      |
|-----------|----------|------------------------------------------------------------------|
| direction | Vertex3D | Indicates the direction the spotlight is pointing                |
| cut       | float    | Defines the size of the spotlight cone                           |
| cutOff    | float    | Sets the light cutoff, controlling the edge softness of the cone |

These values can be modified using the following methods:

| Method                 | Description                                |
|------------------------|--------------------------------------------|
| setDirection(Vertex3D) | Sets the direction of the spotlight        |
| setCut(float)          | Sets the size of the spotlight cone        |
| setCutOff(float)       | Sets the light cutoff (cone edge softness) |

Example of instantiation from a LUA script:

```lua
spot = ObjectFactory.SpotLight(
    Vertex3D.new(10, 20, 30),                               -- position
    Color.new(0.1, 0.1, 0.1),                               -- ambient component
    Color.new(0.2, 0.4, 0.6),                               -- diffuse component
    Color.new(0.5, 0.3, 1.0),                               -- specular component
);
```