#define GL_GLEXT_PROTOTYPES

#include "../../include/OpenGL/ShaderOGLRenderDeferred.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Profiler.h"

ShaderOGLRenderDeferred::ShaderOGLRenderDeferred()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "GBuffer.vs",
        Config::get()->SHADERS_FOLDER + "GBuffer.fs",
        false
    )
{
}

void ShaderOGLRenderDeferred::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    LoadUniforms();

    // Fase 4: attributes 3-6 (mat4 aInstanceModel) sobre un buffer propio, configurados una sola
    // vez aquí -- el binding de atributo sigue siendo válido mientras no cambie el identificador
    // del buffer, así que renderInstanced() solo necesita respecificar el contenido (glBufferData)
    // en cada batch, no repetir este setup.
    glGenBuffers(1, &instanceModelBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
            reinterpret_cast<void*>(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }

    // Instancing de unidades animadas (Fase 1): TBO con las matrices de huesos de todo el batch,
    // leída en el vertex shader como samplerBuffer (GLSL/GBuffer.vs). glTexBuffer() solo hace
    // falta una vez -- la textura sigue reflejando el contenido del buffer aunque éste se
    // respecifique con glBufferData en cada batch (mismo patrón que instanceModelBuffer).
    glGenBuffers(1, &boneMatrixBuffer);
    glGenTextures(1, &boneMatrixTexture);
    glBindTexture(GL_TEXTURE_BUFFER, boneMatrixTexture);
    glBindBuffer(GL_TEXTURE_BUFFER, boneMatrixBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, boneMatrixBuffer);
}

void ShaderOGLRenderDeferred::LoadUniforms()
{
    matrixModelUniform = glGetUniformLocation(programID, "model");
    drawOffsetUniform = glGetUniformLocation(programID, "drawOffset");
    textureDiffuseUniform = glGetUniformLocation(programID, "texture_diffuse");
    textureSpecularUniform = glGetUniformLocation(programID, "texture_specular");
    alphaUniform = glGetUniformLocation(programID, "alpha");
    emissionUniform = glGetUniformLocation(programID, "emission");
    useInstancingUniform = glGetUniformLocation(programID, "useInstancing");
    useSkinningUniform = glGetUniformLocation(programID, "useSkinning");
    bonesPerInstanceUniform = glGetUniformLocation(programID, "bonesPerInstance");
    boneMatricesSamplerUniform = glGetUniformLocation(programID, "boneMatrices");

    // projection/view ya no son uniforms sueltos: se leen de CameraBlock (binding 3),
    // relleno una vez por frame en ComponentRender::UpdateCameraUBO().
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "CameraBlock"), 3);
}

void ShaderOGLRenderDeferred::renderMesh(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo) const
{
    const auto& textures  = o->getModelTextures();
    const auto& specTextures = o->getModelSpecularTextures();
    for (const auto& m: o->getMeshData()) {
        if (!m.visibleInFrustum) continue;
        if (m.materialIndex < 0 || (size_t)m.materialIndex >= textures.size() ||
            (size_t)m.materialIndex >= specTextures.size()) continue;
        auto* tex = textures[m.materialIndex];
        auto* specTex = specTextures[m.materialIndex];
        if (!tex || !specTex) continue;
        render(
            o,
            tex->getOGLTextureID(),
            specTex->getOGLTextureID(),
            useFeedbackBuffer ? m.feedbackBuffer : m.vertexBuffer,
            m.uvBuffer,
            useFeedbackBuffer ? m.feedbackNormalBuffer : m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            o->getAlpha(),
            fbo,
            useFeedbackBuffer ? 0 : m.indexBuffer,
            useFeedbackBuffer ? 0 : m.indexCount
        );
    }
}

void ShaderOGLRenderDeferred::render(
    Object3D *o,
    GLuint texId,
    GLuint specTexId,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int size,
    float alpha,
    GLuint fbo,
    GLuint indexBuffer,
    GLsizei indexCount,
    float emission
) const
{
    auto render = Components::get()->Render();
    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);

    render->ApplyDepthTest(true);
    render->ApplyDepthFunc(GL_LESS);
    render->ApplyBlend(false);
    render->ApplyCulling(true);

    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidthRender(), window->getHeightRender());

    glBindVertexArray(VertexArrayID);

    // projection/view: CameraBlock (binding 3), no hace falta subirlas aqui.
    setBoolUniform(useInstancingUniform, false);
    setBoolUniform(useSkinningUniform, false);
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());
    setVec3Uniform(drawOffsetUniform, o->getDrawOffset().toGLM());

    setFloatUniform(alphaUniform, alpha);
    setFloatUniform(emissionUniform, emission);
    setIntUniform(textureDiffuseUniform, 0);
    setIntUniform(textureSpecularUniform, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specTexId);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    DrawMeshGeometry(GL_TRIANGLES, indexBuffer, indexCount, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
}

// Fase 4: dibuja un run de 2+ entradas de la cola de opacos que comparten todo salvo la matriz
// de modelo (ver ComponentRender::FlushOpaqueQueue) con UNA sola llamada instanciada. Mismo
// cuerpo que render(), salvo: sube instanceModels al buffer persistente en vez de subir "model"
// como uniform, activa useInstancing, y usa DrawMeshGeometryInstanced. alpha/drawOffset se piden
// explícitos (no via Object3D*) porque el run entero ya se verificó que comparte ambos valores --
// no hay un "o" único representativo del batch.
void ShaderOGLRenderDeferred::renderInstanced(
    GLuint texId,
    GLuint specTexId,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int size,
    float alpha,
    const Vertex3D &drawOffset,
    GLuint fbo,
    GLuint indexBuffer,
    GLsizei indexCount,
    const std::vector<glm::mat4> &instanceModels,
    float emission
) const
{
    if (instanceModels.empty()) return;

    auto render = Components::get()->Render();
    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);

    render->ApplyDepthTest(true);
    render->ApplyDepthFunc(GL_LESS);
    render->ApplyBlend(false);
    render->ApplyCulling(true);

    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidthRender(), window->getHeightRender());

    glBindVertexArray(VertexArrayID);

    setBoolUniform(useInstancingUniform, true);
    setBoolUniform(useSkinningUniform, false);
    setVec3Uniform(drawOffsetUniform, drawOffset.toGLM());

    setFloatUniform(alphaUniform, alpha);
    setFloatUniform(emissionUniform, emission);
    setIntUniform(textureDiffuseUniform, 0);
    setIntUniform(textureSpecularUniform, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specTexId);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(instanceModels.size() * sizeof(glm::mat4)),
        instanceModels.data(), GL_DYNAMIC_DRAW);

    DrawMeshGeometryInstanced(GL_TRIANGLES, indexBuffer, indexCount, size, static_cast<GLsizei>(instanceModels.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
}

// Instancing de unidades animadas (Fase 1): igual que renderInstanced(), pero el skinning ya no
// viene horneado en vertexBuffer (feedbackBuffer por-instancia) -- se calcula en GBuffer.vs a
// partir de vertexBuffer/vertexBoneDataBuffer COMPARTIDOS (bind-pose) más las matrices de huesos
// de esta llamada, empaquetadas en boneMatrixBuffer/Texture (TBO) e indexadas por gl_InstanceID.
// Se usa siempre, incluso para un batch de tamaño 1 (evita duplicar la rama no instanciada).
void ShaderOGLRenderDeferred::renderInstancedSkinned(
    GLuint texId,
    GLuint specTexId,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    GLuint vertexBoneDataBuffer,
    int size,
    float alpha,
    const Vertex3D &drawOffset,
    GLuint fbo,
    int bonesPerInstance,
    const std::vector<glm::mat4> &instanceModels,
    const std::vector<glm::mat4> &allBoneMatrices,
    float emission
) const
{
    if (instanceModels.empty() || bonesPerInstance <= 0) return;

    auto render = Components::get()->Render();
    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);

    render->ApplyDepthTest(true);
    render->ApplyDepthFunc(GL_LESS);
    render->ApplyBlend(false);
    render->ApplyCulling(true);

    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidthRender(), window->getHeightRender());

    glBindVertexArray(VertexArrayID);

    setBoolUniform(useInstancingUniform, true);
    setBoolUniform(useSkinningUniform, true);
    setIntUniform(bonesPerInstanceUniform, bonesPerInstance);
    setVec3Uniform(drawOffsetUniform, drawOffset.toGLM());

    setFloatUniform(alphaUniform, alpha);
    setFloatUniform(emissionUniform, emission);
    setIntUniform(textureDiffuseUniform, 0);
    setIntUniform(textureSpecularUniform, 1);
    setIntUniform(boneMatricesSamplerUniform, 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specTexId);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, boneMatrixTexture);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);
    setVAOBoneAttributes(vertexBoneDataBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(instanceModels.size() * sizeof(glm::mat4)),
        instanceModels.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_TEXTURE_BUFFER, boneMatrixBuffer);
    glBufferData(GL_TEXTURE_BUFFER, static_cast<GLsizeiptr>(allBoneMatrices.size() * sizeof(glm::mat4)),
        allBoneMatrices.data(), GL_DYNAMIC_DRAW);

    // indexBuffer=0: la geometria animada nunca paso por el EBO deduplicado de Fase 2.2 (ver
    // comentario en include/3D/Mesh3D.h) -- siempre glDrawArraysInstanced.
    DrawMeshGeometryInstanced(GL_TRIANGLES, 0, 0, size, static_cast<GLsizei>(instanceModels.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(7);
    glDisableVertexAttribArray(8);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    glBindVertexArray(0);
}

void ShaderOGLRenderDeferred::Destroy()
{
    // VAO is size-independent — nothing to destroy on resize
}
