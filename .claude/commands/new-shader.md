# New Post-Process Shader

Create a new post-processing shader effect for Brakeza3D.

## Steps

1. **Read** `include/Render/PostProcessingManager.h` and an existing shader like `src/OpenGL/ShaderOGLTint.cpp` to understand the pattern.

2. **Create GLSL files** in `GLSL/`:
   - `GLSL/$ARGUMENTS.vs` — vertex shader (copy from `GLSL/Tint.vs` as base)
   - `GLSL/$ARGUMENTS.fs` — fragment shader with the effect logic

3. **Create C++ class**:
   - Header: `include/OpenGL/ShaderOGL$ARGUMENTS.h` (subclass `ShaderBaseOpenGLQuad`)
   - Source: `src/OpenGL/ShaderOGL$ARGUMENTS.cpp`
   - Implement: constructor (load GLSL paths), `Render(GLuint fbo, GLuint inputTexture)`, any uniform setters

4. **Register** in `ComponentRender` — add to the scene shader list so it appears in the GUI.

5. **Add GUI controls** if the shader has editable parameters — add to `src/GUI/Objects/ShadersGUI.cpp`.

Usage: `/new-shader FogOfWar`
