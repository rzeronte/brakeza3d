---
name: Rendering Pipeline & Shaders
description: Deferred rendering, G-Buffer layout, shader class hierarchy, GLSL files catalog, postprocessing, shadows, forward mode
type: project
originSessionId: dc70ba7c-6393-464f-b834-22487807b205
---
# Brakeza3D — Rendering Pipeline

## Rendering Mode
- **Primary:** Deferred Rendering (G-Buffer)
- **Optional:** Forward Rendering (`ShaderOGLRenderForward`)
- OpenGL 3.3+ Core Profile

## G-Buffer Layout
4 render targets:
1. **Positions** — World-space position (GL_RGBA16F)
2. **Normals** — World-space normal (GL_RGBA16F)
3. **Albedo + Specular** — RGB albedo + specular strength (GL_RGBA)
4. **Depth** — Depth buffer (GL_DEPTH_COMPONENT)

G-Buffer populated by `ShaderOGLRenderDeferred` (`GLSL/Render.vs/.fs`)

## Lighting Pass
`ShaderOGLShading` / `GLSL/Shading.vs/.fs` — reads G-Buffer, computes ADS lighting for all point/spot lights

## Shadow Mapping
- `ShaderOGLShadowPass` / `GLSL/ShadowPass.vs/.fs`
- PCF 3x3 filtering
- Spot lights: shadow texture array
- Debug: `ShaderOGLShadowPassDebugLight`

## Shader Class Hierarchy
```
ShaderBaseOpenGL (abstract — include/OpenGL/Base/ShaderBaseOpenGL.h)
  ├─ ShaderBaseOpenGLQuad (fullscreen quad base)
  │   ├─ ShaderOGLDepthMap        (GLSL/DepthMap)
  │   ├─ ShaderOGLDOF             (GLSL/DeepOfField)
  │   ├─ ShaderOGLFog             (GLSL/FOG)
  │   ├─ ShaderOGLGrid            (GLSL/Grid)
  │   ├─ ShaderOGLLightPass       (GLSL/LightingPass)
  │   ├─ ShaderOGLLine            (GLSL/Line — 2D)
  │   ├─ ShaderOGLOutline         (GLSL/Outliner)
  │   └─ ShaderOGLTint            (GLSL/Tint)
  ├─ ShaderOGLColor               (GLSL/Color)
  ├─ ShaderOGLImage               (GLSL/Image)
  ├─ ShaderOGLLine3D              (GLSL/Line3D)
  ├─ ShaderOGLParticles           (GLSL/Particle)
  ├─ ShaderOGLPoints              (GLSL/Points)
  ├─ ShaderOGLBonesTransforms     (GLSL/BonesTransforms.vs)
  ├─ ShaderOGLWire                (GLSL/Wireframe)
  ├─ ShaderOGLRenderDeferred      (GLSL/Render — G-Buffer fill)
  ├─ ShaderOGLRenderForward       (forward render variant)
  ├─ ShaderOGLShading             (GLSL/Shading — lighting pass)
  ├─ ShaderOGLShadowPass          (GLSL/ShadowPass)
  └─ ShaderBaseCustom (user-defined GLSL — see shader-chain.md)
```

## Custom Shader System
```
ShaderBaseCustom (Render(fbo, texture) pure virtual)
├─ ShaderBaseCustomOGLCode
│   ├─ ShaderOGLCustomCodePostprocessing  (scene, fullscreen quad)
│   └─ ShaderOGLCustomCodeMesh3D          (object, renders mesh geometry)
└─ ShaderBaseNodes
    ├─ ShaderNodesPostProcessing          (scene, quad via RenderEffect)
    └─ ShaderNodesMesh3D                  (object, via NodeManager::RenderMesh)
```

## GLSL Shader Files (41 files in /GLSL/)
| File | Purpose |
|------|---------|
| Render.vs/.fs | G-Buffer fill (deferred render pass) |
| Shading.vs/.fs | Deferred lighting pass |
| LightingPass.vs/.fs | Point/spot light accumulation |
| ShadowPass.vs/.fs | Shadow depth generation |
| Color.vs/.fs | Solid color mesh |
| Image.vs/.fs | Textured mesh |
| BonesTransforms.vs | Skeletal animation vertex transform |
| DeepOfField.vs/.fs | Depth of field effect |
| FOG.vs/.fs | Fog effect |
| Tint.vs/.fs | Color tint effect |
| Outliner.vs/.fs | Object outline effect |
| Wireframe.vs/.fs | Wireframe overlay |
| DepthMap.vs/.fs | Depth visualization |
| Grid.vs/.fs | Infinite grid |
| Particle.vs/.fs | Particle system |
| Points.vs/.fs | Point cloud |
| Line.vs/.fs | 2D line |
| Line3D.vs/.fs | 3D line |
| Shared/RenderCommons.glsl | Shared utility functions |
| Particles.shader | Custom particle format |

## PostProcessing Manager
- `include/Render/PostProcessingManager.h`
- FBO ping-pong: `pingFBO` / `pongFBO` (GL_RGBA16F + GL_DEPTH_COMPONENT24)
- `processChain(inputTexture, outputFBO)` — iterates shader list, each reads previous output
- Overload: `processChain(shaders, inputTexture, outputFBO)` — external shader vector (for object chains)
- Owned by ComponentWindow; sceneShaders stored in ComponentRender

## Ping-Pong FBO Notes
- Both FBOs have color (GL_RGBA16F) + depth (GL_DEPTH_COMPONENT24) attachments
- Depth is required for object shaders (they call `glEnable(GL_DEPTH_TEST)`)
- Scene postprocessing (fullscreen quads) doesn't use depth but won't break

## Frustum Culling
`ComponentCamera` maintains frustum planes — `Frustum` class in `include/Render/Frustum.h`

**How to apply:** New post-process effects → add `ShaderOGLCustomCodePostprocessing` subclass + GLSL pair + register in ComponentRender's scene shader list. New mesh shaders → use `ShaderOGLCustomCodeMesh3D` or `ShaderNodesMesh3D` subclass.
