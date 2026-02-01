---
title: Global illumination
---

# Global illumination
---

**Brakeza3D** includes a default light in the scene, which can be thought of as the sun.

Essentially, it is a directional light, so you can adjust some of its parameters to achieve the desired
base illumination. Being a global light, it has no position. You can manipulate the components of global
illumination **through the UI** or **using your LUA scripts**.

![Illumination menu options](/img/engine-concepts/01.png)

## ADS Components
---
The ADS (*Ambient*, *Diffuse*, *Specular*) model is **a standard lighting technique** used in computer graphics to simulate how light interacts with objects in a 3D scene. It separates illumination into three components, allowing realistic control of ambient light, direct light reflection, and surface highlights in a simple and efficient way.


## Light types
---

The lighting system allows users to create and configure different types of lights within the scene. All lighting properties **can be edited through the GUI**, with immediate visual feedback.

This enables artists and designers to iterate quickly on scene mood and atmosphere while maintaining technical control over lighting behavior.

In **Brakeza3D**, you will find three types of lights: `global light`, `point lights`, and `spotlights`.

| Light Type   | Description                                  | Position | Shadows | Typical Uses                        |
|--------------|----------------------------------------------|----------|---------|-------------------------------------|
| Global Light | Simulates sunlight and global illumination   | No       | Yes     | Outdoor scenes, daylight lighting   |
| Point Light  | Emits light in all directions from one point | Yes      | No      | Bulbs, candles, small light sources |
| Spotlight    | Emits light in a specific direction          | Yes      | Yes     | Flashlights, stage lights, lamps    |

## Shadow mapping
---

**Brakeza3D** provides `shadow mapping` support for both global directional lights and spotlights, allowing you to **add realistic dynamic shadows to your scenes**. This technique works by rendering the scene from the light’s point of view to generate a depth map, which is then used to determine which areas should be in shadow during the final render pass.

For global illumination, shadow mapping simulates sunlight shadows, producing large-scale, consistent shadow casting across the entire scene. For spotlights, it enables focused, directional shadows, ideal for lamps, flashlights, or stage lighting effects.

![Shadow mapping example](/img/engine-concepts/02.png)

:::note
Currently, point lights do not support shadow mapping, as their omnidirectional nature requires a different and more expensive rendering approach. This design choice ensures optimal performance while maintaining high visual quality where shadows are most impactful.
:::