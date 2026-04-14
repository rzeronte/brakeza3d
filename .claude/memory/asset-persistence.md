---
name: Asset Loading & Persistence
description: Async thread-pool loader, JSON serialization registry, Project/Scene file format, serializer types, asset locations
type: project
originSessionId: dc70ba7c-6393-464f-b834-22487807b205
---
# Brakeza3D — Asset Loading & Persistence

## Asset Loading Libraries
| Asset Type | Library | Formats |
|-----------|---------|--------|
| 3D Models | Assimp | FBX, OBJ, DAE, GLTF, BLEND, 3DS, etc. |
| Textures/Images | SDL2_image | PNG, JPEG, BMP, TGA, etc. |
| Audio | SDL2_mixer | WAV, OGG, FLAC, MP3 |
| JSON | cJSON (embedded C) | Project/Scene files |

## Async Loading (Thread Pool)
All scene loading is non-blocking via thread pool (`ThreadPool` — `include/Render/ThreadPool.h`).

### ThreadJob Types (`include/Threads/ThreadJob*.h`)
| Job | Purpose |
|-----|---------|
| ThreadJobLoadMesh3D | Load static 3D model |
| ThreadJobLoadMesh3DAnimation | Load skeletal animated model |
| ThreadJobLoadImage / Image2D / Image3D | Load image variants |
| ThreadJobLoadImage2DAnimation / Image3DAnimation / Image3DAnimation360 | Animated images |
| ThreadJobLoadLightPoint / LightSpot | Load light objects |
| ThreadJobLoadParticleEmitter | Load particle system |
| ThreadJobLoadObject | Generic object load |
| ThreadJobReadFileScene | Parse scene JSON |
| ThreadJobReadSceneScript | Load scene Lua scripts |
| ThreadJobReadSceneShaders | Load scene shader chain |
| ThreadJobMakeRigidBody | Build Bullet physics body |
| ThreadJobStepSimulation | Physics substep |
| ThreadJobCleanScene | Cleanup scene resources |
| ThreadJobClearScene | Full scene clear |

## JSON Serialization
- **Registry:** `include/Render/JSONSerializerRegistry.h` / `src/Render/JSONSerializerRegistry.cpp`
- **Base template:** `JSONSerializer<T>` in `include/Render/JSONSerializer.h`
- Pattern: registry dispatches to correct serializer by object type string

### Serializer Types (11)
- `Image2DSerializer`, `Image2DAnimationSerializer`
- `Image3DSerializer`, `Image3DAnimationSerializer`, `Image3DAnimation360Serializer`
- `Mesh3DSerializer`, `Mesh3DAnimationSerializer`
- `LightPointSerializer`, `LightSpotSerializer`
- `Object3DSerializer` (base)
- `ParticleEmmitterSerializer`

Files: `include/Serializers/*.h` + `src/Serializers/*.cpp`

## Project File Format (JSON)
Location: `assets/projects/*.json`

Contents:
- name, description
- gravity (vector)
- scene list (references to scene files)
- global scripts
- audio config (FX/music volumes)
- render resolution settings

**Loaders:** `ProjectLoader` + `ProjectChecker` (validation)

## Scene File Format (JSON)
Location: `assets/scenes/*.json`

Contents:
- screenshot preview (base64 or path)
- directional light (full ADS + direction)
- camera (position + rotation)
- object hierarchy (all serialized objects)
- scene-level scripts
- post-processing shader list

**Loaders:** `SceneLoader` + `SceneChecker` (validation)

## Asset Directory Layout
```
assets/
├── projects/      *.json  — project definitions
├── scenes/        *.json  — scene definitions
├── scripts/       *.lua   — Lua scripts
├── models/        (3D models)
├── textures/      (image assets)
└── audio/         (sound files)

GLSL/              — built-in engine shaders
config/            — engine configuration
```

## Path Constants
`include/Render/FilePaths.h` — all path constants centralized here. Reference this when dealing with asset loading paths.

**How to apply:** Adding a new entity type requires: (1) a Serializer subclass, (2) registration in `JSONSerializerRegistry`, (3) a ThreadJob for async loading. Never hardcode asset paths — use `FilePaths.h` constants.
