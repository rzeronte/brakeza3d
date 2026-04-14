# Lessons Learned

## UV-Space vs Screen-Space (Critical!)
- Scene postprocessing: textures are in screen-space, UVs map 1:1 → ping-pong works directly
- Object shaders: render 3D geometry, mesh UVs are texture-space (NOT screen-space)
- **NEVER replace DIFFUSE/Mesh Texture with chain output** - UV coords don't match screen positions → BLACK output
- Tested and confirmed: feeding chain texture to Mesh Texture → all black because mesh UVs read from wrong positions
- Chain texture must be sampled with `gl_FragCoord.xy / textureSize(tex, 0)` (screen-space)
- Access chain output: SCENE data type (code shaders) or Internal Texture node (node shaders)
- RenderEffect() can't be used for intermediate mesh shaders - vertex shader is compiled for 3D (SHADER_NODE_OBJECT), not 2D quad

## ShaderNodesMesh3D::RenderMesh vs ShaderNodeEditorManager::RenderMesh
- ShaderNodesMesh3D::RenderMesh() calls `RenderEffect(fbo)` which renders a fullscreen QUAD → causes white screen
- ShaderNodeEditorManager::RenderMesh() renders actual mesh geometry with depth test → correct
- Always use `GetNodeManager()->RenderMesh()` for actual mesh rendering from ShaderNodesMesh3D

## PostProcessingManager Reference Member
- `std::vector<ShaderBaseCustom*> &postProcessingShaders` is a REFERENCE (can't re-seat)
- Solution: add overloaded processChain that accepts external vector as parameter
- The existing processChain delegates to the overload

## ShaderBaseNodes::Render() is a no-op
- Base class Render() is empty
- ShaderNodesMesh3D did NOT override it before our changes
- That's why RunObjectShaders had manual dynamic_cast handling
- We added proper Render() override that delegates to NodeManager::RenderMesh()

## Object Chain: First shader gets non-zero inputTexture
- RunObjectShaders originally passed GBuffer.albedo as inputTexture → non-zero!
- The `texture != 0` check in Render() then treats the FIRST shader as chained
- Fix: pass 0 from RunObjectShaders so first shader uses its own material texture
- processChain scene shaders still pass the real input texture (unaffected)

## Ping-Pong FBOs need depth for object shaders
- Originally ping-pong FBOs had only color attachment (GL_RGBA16F)
- Object shaders call glEnable(GL_DEPTH_TEST) but without depth attachment → depth test fails
- Fix: add GL_DEPTH_COMPONENT24 renderbuffer to each ping-pong FBO
- Scene postprocessing (fullscreen quads) doesn't need depth, but it doesn't hurt
