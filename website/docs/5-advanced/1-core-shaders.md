---
title: Core shaders
description: Advanced documentation for Brakeza3D core shaders and internal rendering pipeline.
sidebar_position: 1
---

# Core Shaders
---

This section provides detailed documentation about **Brakeza3D's internal shaders**. These shaders power the engine's rendering pipeline and can be found in the `/GLSL` folder.

:::warning
Modifying core shaders requires a solid understanding of GLSL and the rendering pipeline. Incorrect changes may cause visual artifacts or crashes.
:::

## Shader Architecture
---

**Brakeza3D** uses a combination of forward and deferred rendering techniques. Each shader consists of:

- **Vertex Shader (.vs)**: Transforms vertices and passes data to the fragment shader
- **Fragment Shader (.fs)**: Calculates the final pixel color

## Mesh/Object Shaders
---

These shaders handle the rendering of 3D objects in the scene.

| Shader | Files | Description |
|--------|-------|-------------|
| **Render** | `Render.vs` / `Render.fs` | Main forward rendering shader for `Mesh3D` objects. Handles lighting, textures, and material properties |
| **GBuffer** | `GBuffer.vs` / `GBuffer.fs` | Geometry buffer pass for deferred rendering. Outputs position, normal, and albedo to multiple render targets |
| **LightingPass** | `LightingPass.vs` / `LightingPass.fs` | Deferred lighting calculations using G-buffer data |
| **Outliner** | `Outliner.vs` / `Outliner.fs` | Renders a colored outline around selected objects in the editor |
| **Color** | `Color.vs` / `Color.fs` | Renders a `Mesh3D` with a solid color (no textures) |
| **Wireframe** | `Wireframe.vs` / `Wireframe.fs` | Renders object edges as wireframe |
| **Points** | `Points.vs` / `Points.fs` | Renders vertices as points |
| **Shading** | `Shading.vs` / `Shading.fs` | Renders an object to a buffer for use as a mask |

## 2D/Utility Shaders
---

| Shader | Files | Description |
|--------|-------|-------------|
| **Image** | `Image.vs` / `Image.fs` | Renders 2D images on screen (`Image2D` objects) |
| **Line** | `Line.vs` / `Line.fs` | Draws 2D lines on screen |
| **Line3D** | `Line3D.vs` / `Line3D.fs` | Draws 3D lines in world space (used for debugging) |
| **Grid** | `Grid.vs` / `Grid.fs` | Renders the editor grid |
| **Particle** | `Particle.vs` / `Particle.fs` | Renders particles for `ParticleEmitter` objects |

## Shadow Shaders
---

| Shader | Files | Description |
|--------|-------|-------------|
| **ShadowPass** | `ShadowPass.vs` / `ShadowPass.fs` | Generates shadow maps from light perspective |
| **ShadowPassDebugLight** | `ShadowPassDebugLight.vs` / `ShadowPassDebugLight.fs` | Visualizes shadow maps for debugging |
| **DepthMap** | `DepthMap.vs` / `DepthMap.fs` | Renders scene depth information |

## Post-Processing Shaders
---

| Shader | Files | Description |
|--------|-------|-------------|
| **DeepOfField** | `DeepOfField.vs` / `DeepOfField.fs` | Depth of field blur effect based on distance from focus |
| **FOG** | `FOG.vs` / `FOG.fs` | Distance-based fog effect |
| **Tint** | `Tint.vs` / `Tint.fs` | Applies a color tint with alpha blending to the entire screen |

## Animation Shaders
---

| Shader | Files | Description |
|--------|-------|-------------|
| **BonesTransforms** | `BonesTransforms.vs` | Handles skeletal animation bone transformations |

## Common Uniforms
---

Most shaders receive these standard uniforms from the engine:

### Transformation Matrices

| Uniform | Type | Description |
|---------|------|-------------|
| `model` | `mat4` | Model matrix (object to world space) |
| `view` | `mat4` | View matrix (world to camera space) |
| `projection` | `mat4` | Projection matrix (camera to clip space) |

### Lighting Uniforms

| Uniform | Type | Description |
|---------|------|-------------|
| `viewPos` | `vec3` | Camera position in world space |
| `lightDirection` | `vec3` | Global directional light direction |
| `ambientColor` | `vec3` | Global ambient light color |
| `diffuseColor` | `vec3` | Global diffuse light color |
| `specularColor` | `vec3` | Global specular light color |

### Time Uniforms

| Uniform | Type | Description |
|---------|------|-------------|
| `u_Time` | `float` | Current time (for animated effects) |
| `delta_time` | `float` | Time since last frame |
| `execution_time` | `float` | Total execution time |

## Shared Code
---

The `/GLSL/Shared` folder contains common GLSL functions that can be included in multiple shaders:

- Lighting calculations
- Shadow sampling functions
- Utility functions

## Modifying Core Shaders
---

If you need to modify core shaders:

1. **Backup first**: Copy the original shader before making changes
2. **Test incrementally**: Make small changes and test frequently
3. **Check console**: Shader compilation errors appear in the log
4. **Use debug mode**: Enable visual debugging to verify changes

### Example: Adding a Custom Effect to Render.fs

```glsl
// In Render.fs, after calculating finalColor:

// Custom desaturation effect
float gray = dot(finalColor.rgb, vec3(0.299, 0.587, 0.114));
finalColor.rgb = mix(finalColor.rgb, vec3(gray), 0.5); // 50% desaturation
```

## Performance Considerations
---

- **Minimize texture samples**: Each `texture()` call has a cost
- **Avoid branching**: Use `mix()` and `step()` instead of `if` statements
- **Use appropriate precision**: `mediump` for colors, `highp` for positions
- **Batch similar operations**: Calculate common values once and reuse
