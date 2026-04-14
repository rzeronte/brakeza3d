# Debug Rendering Issue

Diagnose a rendering problem in Brakeza3D.

Problem description: **$ARGUMENTS**

## Diagnostic Checklist

### 1. Black / invisible output?
- Check if it's an **object shader**: mesh UVs are texture-space, NOT screen-space. Never sample a ping-pong chain texture via mesh UV — use `gl_FragCoord.xy / textureSize(tex, 0)` instead.
- Check ping-pong FBO has **depth attachment** (`GL_DEPTH_COMPONENT24`) — required for object shaders that call `glEnable(GL_DEPTH_TEST)`.
- Check that `RunObjectShaders` passes `0` as inputTexture for the first shader (not GBuffer.albedo).

### 2. White screen from node shader?
- `ShaderNodesMesh3D::Render()` must call `GetNodeManager()->RenderMesh()` — NOT `this->RenderMesh()` (which calls `RenderEffect()` = fullscreen quad → white screen).

### 3. Shader not appearing in chain?
- Check registration in `ComponentRender`'s shader list.
- Check `ShaderBaseCustom::Render()` — base class is a no-op, subclass must override.

### 4. Frame order issue?
- Object shaders run at step 2 (OnUpdateComponents).
- Scene postprocessing runs at step 6 (PostProcessingShadersChain).
- Inserting at wrong step causes timing bugs.

### 5. General steps
Read `src/Components/ComponentRender.cpp` to trace the full render path for the affected object type.
Check `src/Render/PostProcessingManager.cpp` for ping-pong logic.
Add `glGetError()` calls around suspect GL calls.
