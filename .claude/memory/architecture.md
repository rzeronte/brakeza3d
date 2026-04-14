---
name: Engine Architecture Overview
description: Top-level subsystems, design patterns, entry point, component manager, threading model, and hard limits
type: project
originSessionId: dc70ba7c-6393-464f-b834-22487807b205
---
# Brakeza3D — Engine Architecture

## Entry Point & Main Loop
- `main.cpp` → creates `Brakeza` singleton, calls engine loop
- `include/Brakeza.h` / `src/Brakeza.cpp` — engine singleton, main loop, component lifecycle

## Component Manager (Singleton)
`include/Components/Components.h` — `Components::get()` — owns all 7 core components:

| Component | File | Responsibility |
|-----------|------|----------------|
| ComponentRender | ComponentRender.h/.cpp | Graphics pipeline, deferred/forward, postprocessing, shadows |
| ComponentCamera | ComponentCamera.h/.cpp | Perspective projection, view/projection matrices, frustum |
| ComponentCollisions | ComponentCollisions.h/.cpp | Bullet Physics integration |
| ComponentInput | ComponentInput.h/.cpp | Keyboard, mouse, gamepad |
| ComponentSound | ComponentSound.h/.cpp | SDL2_mixer audio |
| ComponentWindow | ComponentWindow.h/.cpp | SDL2 window, VSync, resolution |
| ComponentScripting | ComponentScripting.h/.cpp | Lua scripting lifecycle |

## Main Loop Order (per frame)
1. `PreUpdateComponents` — input polling, physics prepare
2. `OnUpdateComponents` — object logic + Mesh3D RunObjectShaders (object shader chain HERE)
3. LightPass — deferred lighting
4. `PostUpdateComponents` — transparent objects
5. `FlipBuffersToGlobal` — compose layers
6. `PostProcessingShadersChain` — scene-level shader chain (HERE)
7. `FlipGlobalToWindow` — final blit
8. ImGui render pass

## Design Patterns
- **Singleton**: `Components`, `Brakeza`
- **Component Architecture**: 7 core components (not ECS, components are singletons)
- **Registry**: Serialization/Deserialization dispatch (`JSONSerializerRegistry`)
- **Strategy**: Shader implementations (interchangeable via polymorphism)
- **Object Pool / Thread Pool**: Async asset loading
- **Factory**: `ObjectFactory` for Lua-side object creation

## Threading Model
Two thread pools:
- `pool` — general computation (scene loading, physics substep)
- `poolImages` — specialized image loading

15+ ThreadJob types (see `include/Threads/ThreadJob*.h`). All async, non-blocking.

## Rendering Layer Composition (5 layers)
1. Background FBO
2. Scene FBO (main deferred/forward render)
3. Foreground FBO
4. UI FBO (ImGui overlay)
5. Global FBO (final composite)

## Hard Engine Limits
| Resource | Limit |
|----------|-------|
| Point lights | 64 |
| Spot lights | 64 |
| Particles per emitter | 512 |
| Bones per mesh | 100 |
| Bones per vertex | 4 |
| Object entity types | 12 |
| Shader node types | 30+ |

## Key Top-Level Files
- `main.cpp` — entry point
- `include/Brakeza.h` / `src/Brakeza.cpp` — engine singleton
- `include/Config.h` / `src/Config.cpp` — global config
- `include/Render/FilePaths.h` — path constants

**Why:** The component singleton pattern and 5-layer FBO composition are non-obvious. Knowing the frame order prevents wrong insertion points for new features.
