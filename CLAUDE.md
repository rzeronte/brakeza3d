# Brakeza3D — Claude Code Instructions

@.claude/MEMORY.md

## Project
C++ 3D/2D game engine (OpenGL 3.3+, deferred rendering, ImGui editor, Lua scripting).
Version 0.26.1+, GPLv3. Build: CMake C++17, Windows/CLion primary.

## Build
```bash
# From CLion or terminal (MinGW):
cmake -B cmake-build-debug-mingw-brakezabundle -G "MinGW Makefiles" .
cmake --build cmake-build-debug-mingw-brakezabundle
```
Executable: `Brakeza3D`. Assets must be in `assets/` relative to the executable.

## Architecture at a Glance

### Frame Loop Order (critical — wrong insertion = bugs)
1. PreUpdateComponents (input, physics prep)
2. OnUpdateComponents → `Mesh3D::RunObjectShaders()` ← **object shader chain here**
3. LightPass (deferred ADS lighting)
4. PostUpdateComponents (transparent objects)
5. FlipBuffersToGlobal (layer composition)
6. PostProcessingShadersChain ← **scene shader chain here**
7. FlipGlobalToWindow → ImGui

### Key Entry Points
| Task | File |
|------|------|
| Engine loop | `src/Brakeza.cpp` |
| All components | `include/Components/Components.h` → `Components::get()` |
| Render pipeline | `src/Components/ComponentRender.cpp` |
| Post-processing | `include/Render/PostProcessingManager.h` |
| Scene/Project load | `src/Loaders/SceneLoader.cpp`, `ProjectLoader.cpp` |
| Shader node editor | `src/GUI/ShaderNodeEditorManager.cpp` |
| Lua bridge | `include/LUA/BrakezaLuaBridge.h` |
| Path constants | `include/Render/FilePaths.h` — use these, never hardcode paths |

### Entity Hierarchy
```
Collider → Object3D → Mesh3D / Mesh3DAnimation / LightPoint / LightSpot /
           ParticleEmitter / Image3D / Image2D / Image3DAnimation / Swarm
```

### Shader Chain (ping-pong FBOs)
- **Scene shaders** (PostProcessingManager): screen-space, fullscreen quad, UVs = screen coords
- **Object shaders** (Mesh3D::customShaders): 3D geometry, mesh UVs = texture-space
- **NEVER** feed ping-pong chain texture to a mesh diffuse slot → black output (UV mismatch)
- Chain texture in code shaders: `SCENE` data type (`gl_FragCoord.xy / textureSize`)
- Chain texture in node shaders: `Internal Texture (Color)` node
- Both chains share the same ping-pong FBOs safely (they run at different frame steps)
- Ping-pong FBOs have both color (GL_RGBA16F) **and** depth (GL_DEPTH_COMPONENT24) attachments

### Adding New Entity Types (checklist)
1. Subclass `Object3D` → `include/3D/` + `src/3D/`
2. Add `JSONSerializer` subclass → `include/Serializers/` + `src/Serializers/`
3. Register in `JSONSerializerRegistry` → `src/Render/JSONSerializerRegistry.cpp`
4. Add GUI inspector → `src/GUI/Objects/`
5. Add `ThreadJob` for async loading → `include/Threads/`

### Adding New Post-Process Shaders
1. Add GLSL pair to `GLSL/` (`MyEffect.vs` + `MyEffect.fs`)
2. Subclass `ShaderBaseCustomOGLCode` (code) or `ShaderBaseNodes` (node)
3. Register in `ComponentRender`'s scene shader list

## Hard Limits
| Resource | Limit |
|----------|-------|
| Point lights | 64 |
| Spot lights | 64 |
| Particles/emitter | 512 |
| Bones/mesh | 100 |
| Bones/vertex (weighted) | 4 |

## Physics
- Bullet Physics. Body modes: **Dynamic** (gravity), **Kinematic** (character), **Ghost** (trigger)
- Collision groups: Player, Enemy, Projectile, ProjectileEnemy, Health, Weapon
- Physics runs on worker thread (`ThreadJobStepSimulation`)

## Lua Scripting
- Lua 5.2 + Sol2. Scripts in `assets/scripts/*.lua`
- Lifecycle: `onStart()` → `preUpdate()` → `onUpdate()` → `postUpdate()`
- Collision callbacks: `onCollisionEnter/Stay/Exit(info)`
- Math types: `Vertex3D.new(x,y,z)`, `M3`, `Color.new(r,g,b,a)`
- Engine singleton: `brakeza`. Components: `Components.get()`

## Known Issues (todo.md)
- Removing a shader from an object doesn't remove it from code/node editor in live mode
- AutoFix in NodeEditor fails sometimes
- `DrawEditBonesMappingWindow` needs refactor into its own window

## Conventions
- No hardcoded asset paths — use `FilePaths.h`
- JSON config uses embedded cJSON library
- Async loading always via ThreadPool jobs — never load assets on main thread
- ImGui addons in `src/GUI/AddOns/` follow the `GUIAddon*` naming pattern
- Shader node types registered in `ShaderNodeEditorManager`
