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
