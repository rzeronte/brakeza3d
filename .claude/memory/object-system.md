---
name: Object/Entity System
description: Full entity hierarchy, Object3D base, all 12 entity types with key properties, parent-child system
type: project
originSessionId: dc70ba7c-6393-464f-b834-22487807b205
---
# Brakeza3D — Object/Entity System

## Class Hierarchy
```
Collider (physics base — include/Render/Collider.h)
  └─ Object3D (base entity — include/3D/Object3D.h)
      ├─ Mesh3D                  (static model)
      ├─ Mesh3DAnimation         (skeletal animated model)
      ├─ LightPoint              (omnidirectional light)
      ├─ LightSpot               (spotlight + shadow)
      ├─ ParticleEmitter         (particle system)
      ├─ Image3D                 (3D billboard sprite)
      ├─ Image3DAnimation        (animated 3D sprite)
      ├─ Image3DAnimation360     (360-degree animated billboard)
      ├─ Image2D                 (2D screen-space sprite)
      ├─ Image2DAnimation        (animated 2D sprite)
      ├─ Swarm                   (behavior/flocking group)
      └─ Cube3D / Cone3D         (primitives)
```
All files: `include/3D/*.h` + `src/3D/*.cpp`

## Object3D — Base Entity
Properties: position, rotation (`M3` matrix), scale, alpha, enabled/disabled  
Features:
- Parent-child hierarchy: `AttachObject()`, `setParent()`
- Lua scripting: `AttachScript()`, multiple scripts per object
- Physics collider attachment
- LookAt() utility
- Color-buffer picking (unique color ID per object)

## Mesh3D — Static 3D Model
- Model loading via Assimp (FBX, OBJ, DAE, GLTF, etc.)
- Material: diffuse texture, specular, shininess
- **Custom shader chain** (`customShaders` vector — Code + Node shaders)
- Shader chain uses shared ping-pong FBOs from `PostProcessingManager`
- `MoveShaderUp()` / `MoveShaderDown()` for reordering
- Serialized by `Mesh3DSerializer`

## Mesh3DAnimation — Skeletal Animated Model
- Up to 100 bones, 4 bones per vertex (weighted interpolation)
- Keyframe interpolation via Assimp animation data
- Per-bone colliders: sphere, box, capsule (for hit detection)
- `AssimpAnimationService` extracts bone/keyframe data

## LightPoint — Omnidirectional Light
- ADS: Ambient, Diffuse, Specular colors
- Attenuation: constant, linear, quadratic factors
- Max 64 per scene (GPU uniform array limit)

## LightSpot — Spotlight
- Position + direction + inner/outer cutoff angles
- Shadow mapping via texture array
- Max 64 per scene

## ParticleEmitter
- Max 512 particles per emitter
- Billboarding (always faces camera)
- Depth sorting (back-to-front for alpha blending)
- Modes: DEFAULT (continuous), EXPLOSION (burst)
- Parameters: gravity, velocity, lifespan, decay
- Shader: `ShaderOGLParticles` (`GLSL/Particle.vs/.fs`)

## Image3D — 3D Billboard Sprite
- Quad mesh oriented to camera
- Textured with alpha transparency

## Image2D — 2D Screen-Space Sprite
- Screen-space positioning
- Used for HUD/UI elements

## Scene Object Management
All scene objects live in a flat list managed by `SceneLoader`. The object hierarchy (parent-child) is maintained via `Object3D::parent` pointer. During serialization, the full hierarchy is preserved in JSON.

**How to apply:** When adding new entity types, subclass `Object3D`, add a corresponding `Serializer`, register in `JSONSerializerRegistry`, add GUI inspector in `/src/GUI/Objects/`, add `ThreadJob` for async loading.
