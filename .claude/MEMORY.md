# Brakeza3D — Auto-Memory

## Engine & Build
- C++ 17, OpenGL 3.3 Core, CMake + MinGW/CLion, Windows primary. Exe: `Brakeza3D`. Assets relative to exe.
- Entry: `main.cpp` → `Brakeza` singleton (`src/Brakeza.cpp`). All subsystems via `Components::get()` (`include/Components/Components.h`).
- 7 components: ComponentRender, ComponentCamera, ComponentCollisions, ComponentInput, ComponentSound, ComponentWindow, ComponentScripting.
- Ext libs: SDL2, SDL2_image, SDL2_mixer, SDL2_ttf, OpenGL+GLEW, Bullet Physics, Assimp, Lua 5.2, cURL. Embedded: ImGui, Sol2, ImGui-node-editor, cxxxopts.

## Frame Loop Order (insert code at wrong step = bugs)
1. PreUpdateComponents — input, physics prep
2. OnUpdateComponents → `Mesh3D::RunObjectShaders()` ← **object shader chain HERE**
3. LightPass (deferred ADS)
4. PostUpdateComponents (transparent)
5. FlipBuffersToGlobal (5-layer composition)
6. PostProcessingShadersChain ← **scene shader chain HERE**
7. FlipGlobalToWindow → ImGui

## Entity Hierarchy (12 types, all in `include/3D/` + `src/3D/`)
`Collider → Object3D → Mesh3D / Mesh3DAnimation / LightPoint / LightSpot / ParticleEmitter / Image3D / Image3DAnimation / Image3DAnimation360 / Image2D / Image2DAnimation / Swarm / Cube3D`

## Shader Chain — Critical Rules
- **Scene shaders** (PostProcessingManager ping-pong): fullscreen quad, UVs = screen-space → `gl_FragCoord.xy / textureSize`
- **Object shaders** (Mesh3D::customShaders): 3D geometry, UVs = texture-space
- **NEVER** feed ping-pong chain texture to mesh diffuse/UV slot → black output (coords don't match)
- Access chain in code shader: `SCENE` data type. In node shader: `Internal Texture (Color)` node.
- Ping-pong FBOs need `GL_DEPTH_COMPONENT24` — object shaders call `glEnable(GL_DEPTH_TEST)`
- `ShaderNodesMesh3D` must call `GetNodeManager()->RenderMesh()` NOT `this->RenderMesh()` (→ white screen)
- Object chain first shader must receive `inputTexture=0` (not GBuffer.albedo)

## Rendering Pipeline
- Deferred render: G-Buffer = positions + normals + albedo+specular + depth (`ShaderOGLRenderDeferred` → `GLSL/Render.vs/.fs`)
- Lighting pass: `ShaderOGLShading` (`GLSL/Shading.vs/.fs`), ADS, max 64 point + 64 spot lights
- Shadows: PCF 3x3, spot lights use texture array (`ShaderOGLShadowPass`)
- Post-process base: `ShaderBaseOpenGLQuad`. Custom: `ShaderBaseCustomOGLCode` (code) / `ShaderBaseNodes` (nodes)
- Key files: `include/Render/PostProcessingManager.h`, `src/Components/ComponentRender.cpp`

## Scripting (Lua 5.2 + Sol2)
- Scripts: `assets/scripts/*.lua`. Bridge: `include/LUA/BrakezaLuaBridge.h`
- Lifecycle per object: `onStart() → preUpdate() → onUpdate() → postUpdate()` + `onCollisionEnter/Stay/Exit(info)`
- Types: `Vertex3D.new(x,y,z)`, `M3`, `Color.new(r,g,b,a)`. Engine: `brakeza`. Components: `Components.get()`

## Physics (Bullet)
- `ComponentCollisions`. Modes: Dynamic (gravity), Kinematic (character), Ghost (trigger)
- Shapes: Box, Capsule, TriangleMesh. Groups: Player, Enemy, Projectile, ProjectileEnemy, Health, Weapon
- Physics on worker thread (`ThreadJobStepSimulation`). Debug toggle in toolbar.

## Persistence & Loading
- All scene loading async via ThreadPool (15+ ThreadJob types in `include/Threads/`)
- JSON serialization: `JSONSerializerRegistry` dispatches to 11 serializer types (`include/Serializers/`)
- Project: `assets/projects/*.json` (gravity, scenes, global scripts, resolution)
- Scene: `assets/scenes/*.json` (dir light, camera, object hierarchy, scripts, shader chain)
- Path constants: `include/Render/FilePaths.h` — always use these, never hardcode

## GUI / Editor
- ImGui + SDL2 backend. 20+ dockable windows. Manager: `src/GUI/GUIManager.cpp`
- Shortcuts: F9 Scene Objects, F10 Object Properties, F11 Project Settings, F12 File Browser
- Shader node editor: `src/GUI/ShaderNodeEditorManager.cpp`. 30+ node types. Generates GLSL, real-time compile.
- Addons: `src/GUI/AddOns/GUIAddon*.cpp`. Object inspectors: `src/GUI/Objects/`

## Checklist: Add New Entity Type
1. Subclass `Object3D` in `include/3D/` + `src/3D/`
2. Add `JSONSerializer` subclass in `include/Serializers/` + `src/Serializers/`
3. Register in `src/Render/JSONSerializerRegistry.cpp`
4. Add GUI inspector in `src/GUI/Objects/`
5. Add `ThreadJob` in `include/Threads/`

## Known Issues (todo.md)
- Removing shader from object doesn't remove it from code/node editor in live mode (critic)
- AutoFix NodeEditor fails sometimes (minor)
- `DrawEditBonesMappingWindow` needs refactor (minor)

## Detail Files (versioned in repo at `.claude/memory/`)
- `.claude/memory/shader-chain.md` — full ping-pong chain internals + class hierarchy
- `.claude/memory/lessons.md` — postmortem pitfalls with code examples
- `.claude/memory/rendering-pipeline.md` — full GLSL catalog (41 files), shader class tree
- `.claude/memory/object-system.md` — all 12 entity types detailed
- `.claude/memory/scripting-system.md` — full Lua API reference
- `.claude/memory/physics-system.md` — Bullet config, per-bone colliders, IK objects
- `.claude/memory/gui-editor.md` — all editor windows, node types, addon architecture
- `.claude/memory/asset-persistence.md` — all ThreadJob types, serializer list
- `.claude/memory/build-deps.md` — CMake flags, all deps, platform notes
