# Shader Chain Architecture

## Scene-Level Chain (PostProcessingManager)
- FBO ping-pong: pingFBO/pongFBO (GL_RGBA16F, fullscreen)
- Each shader renders fullscreen quad reading previous output
- processChain(inputTexture, outputFBO) - last shader writes to outputFBO
- Supports: ShaderOGLCustomCodePostprocessing + ShaderNodesPostProcessing
- Owned by ComponentWindow, sceneShaders stored in ComponentRender

## Object-Level Chain (Mesh3D)
- Uses SAME PostProcessingManager (shared ping-pong FBOs)
- processChain(shaders, inputTexture, outputFBO) overload added
- Supports: ShaderOGLCustomCodeMesh3D + ShaderNodesMesh3D
- customShaders stored in Mesh3D
- MoveShaderUp/MoveShaderDown for reordering
- Serialization: Mesh3DSerializer handles save/load (no changes needed)

## Key Difference: Scene vs Object Shaders
- Scene shaders: 2D image filter (texture → quad → texture). UVs = screen-space.
- Object shaders: 3D geometry render (vertices → triangles → FBO). UVs = mesh texture-space.
- **CANNOT replace DIFFUSE with chain texture** for object shaders - UV coordinates don't match screen-space!

## How Chain Texture is Accessed

### Code Shaders (ShaderOGLCustomCodeMesh3D)
- `setTextureResult(texture)` stores chain texture in `textureResult`
- Available as uniform type **SCENE** in `setDataTypesUniforms()`
- Fragment shader samples with screen-space UVs: `gl_FragCoord.xy / textureSize(sceneTex, 0)`

### Node Shaders (ShaderNodesMesh3D)
- processChain calls `UpdateInternalTextures(chainTexture, depthTexture)` on the NodeManager
- Available as **Internal Texture (Color)** node in the node graph
- Must be sampled with screen-space coordinates

## Render Pipeline Order
1. PreUpdateComponents
2. OnUpdateComponents → Mesh3D::onUpdate() → RunObjectShaders() (object chain HERE)
3. LightPass
4. PostUpdateComponents (transparent)
5. FlipBuffersToGlobal
6. PostProcessingShadersChain (scene chain HERE)
7. FlipGlobalToWindow

Object chain (step 2) and scene chain (step 6) don't overlap → safe to share ping-pong FBOs.

## Class Hierarchy
```
ShaderBaseCustom (Render(fbo, texture) pure virtual)
├── ShaderBaseCustomOGLCode
│   ├── ShaderOGLCustomCodePostprocessing (scene, renders quad)
│   └── ShaderOGLCustomCodeMesh3D (object, renders mesh geometry)
└── ShaderBaseNodes
    ├── ShaderNodesPostProcessing (scene, renders quad via RenderEffect)
    └── ShaderNodesMesh3D (object, renders mesh via NodeManager::RenderMesh)
```

## ShaderNodesMesh3D::Render()
- Must use `GetNodeManager()->RenderMesh()` (ShaderNodeEditorManager version)
- NOT `this->RenderMesh()` (ShaderNodesMesh3D version) which calls RenderEffect (fullscreen quad) → white screen
- ShaderNodeEditorManager::RenderMesh enables depth test, proper vertex attribs (pos=vec4, normal=vec3, uv=vec2)

## Node shaders (ShaderNodeEditorManager) — rules for editing/adding nodes
- **Threading**: the manager's constructor touches no GL. The quad (VAO/VBO) and the first compile happen in `Update()`
  (main thread). `ShaderNodesMesh3D` creates + loads its manager in `PrepareMainThread()` (mesh loading runs on a worker
  thread: never create the manager there — OpenGL and ImGui-node-editor are main-thread only).
- **Ownership**: `ShaderBaseNodes` owns and deletes its manager. `Image`s of Internal/Mesh Texture nodes wrap engine
  textures (`filepath == "[System Texture]"`): always release them through `ReleaseNodeImage` (zeroes the GL id first,
  otherwise `~Image` would `glDeleteTextures` the engine's texture). They are never serialized.
- **Parameters**: each `NodeType` with editable data implements `SerializeParams/DeserializeParams` (stored in the node's
  `userData` JSON: Color r/g/b, FloatValue value, Mix factor, Smoothstep edge0/edge1, Contrast contrast, Saturation
  saturation, Vignette intensity/smoothness, textureType). Pass them to GLSL with `editor->AddUniform(name, n, floatPtr)`
  (read from the node's userData each draw) — do NOT bake them as literals and do NOT call `SetNeedsRecompile()` from the
  UI: recompile only when the graph changes (links, nodes, images).
- **Multiple outputs**: `TraverseNode` caches per *output pin*. `GenerateCode(node, sourcePin, ...)` must emit only the
  variable of `sourcePin` (own variable name per pin, e.g. `var5` / `var5_a`) — it is called once per used output.
- **Unique GLSL names**: every temporary a node declares must be prefixed with `var<id>` (two nodes of the same type must
  coexist in one graph).
- **Pin types**: `Vector` = vec3 (colors), `Vec2` = UV, `Float`. `CanCreateLink` also rejects cycles; an input holds a
  single link (a new one replaces the old); links are stored Output→Input. Ids from a loaded file are reserved first
  (`LoadFromJSON` pre-scan) and pins keep the code's names/types, taking only the ids from the file.
- A graph that fails to compile shows the error in the node editor/debug panel (`GetLastCompileError`); post-process
  nodes then pass the input texture through instead of leaving the chain black.
- **GLSL helper functions** (`AddGlslHelper`, e.g. Noise) are emitted in the order they were requested (a vector, not a
  map): GLSL needs declare-before-use and a helper may depend on an earlier one (`noiseFractal` -> `noiseValue`).
- **Check without launching the engine**: `python tools/check_node_shaders.py` compiles the REAL node headers and the real
  `GenerateShaderCode/TraverseNode/AddUniform/AddGlslHelper` text from `ShaderNodeEditorManager.cpp` into a small harness,
  generates the GLSL of every example in `assets/shaders/Examples/Node*`, checks the SerializeParams/DeserializeParams
  round trip and pin counts, then compiles+links every shader on the real GL driver. Run it after touching any node or the
  manager's generation code (needs a configured `cmake-build-*` dir for flags; it does not run cmake).

