//
// Created by Eduardo on 13/11/2025.
//

#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include "../../include/Config.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/Profiler.h"
#include "../../include/Render/Frustum.h"
#include <algorithm>

namespace {
    // Entrada local de agrupación para drawCastersInstanced -- el shadow pass es un pase de solo
    // profundidad (sin textura/alpha/drawOffset), así que no reutiliza RenderQueueEntry.
    struct ShadowQueueEntry {
        GLuint vertexBuffer = 0;
        GLuint uvBuffer = 0;
        GLuint normalBuffer = 0;
        GLuint indexBuffer = 0;
        GLsizei indexCount = 0;
        int vertexCount = 0;
        glm::mat4 model{};
    };
}

ShaderOGLShadowPass::ShaderOGLShadowPass()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "ShadowPass.vs",
        Config::get()->SHADERS_FOLDER + "ShadowPass.fs",
        false
    )
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
}

void ShaderOGLShadowPass::LoadUniforms()
{
    matrixViewUniform = glGetUniformLocation(programID, "lightSpaceMatrix");
    matrixModelUniform = glGetUniformLocation(programID, "model");
    useInstancingUniform = glGetUniformLocation(programID, "useInstancing");
}

void ShaderOGLShadowPass::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    ResetFramebuffers();

    // Fase 4: attributes 3-6 (mat4 aInstanceModel) sobre un buffer propio, configurados una sola
    // vez -- mismo patrón que ShaderOGLRenderDeferred::PrepareMainThread(). El VAO de esta clase
    // se crea en el constructor (no aquí), así que se re-bindea explícitamente antes de tocarlo.
    glBindVertexArray(VertexArrayID);
    glGenBuffers(1, &instanceModelBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
            reinterpret_cast<void*>(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }
}

void ShaderOGLShadowPass::renderMeshIntoArrayTextures(Mesh3D *o, bool feedbackFBO, LightSpot* light, int indexLight ) const
{
    for (const auto& m: o->getMeshData()) {
        if (!m.visibleInFrustum) continue;
        renderIntoArrayDepthTextures(
            o,
            light,
            feedbackFBO ? m.feedbackBuffer : m.vertexBuffer,
            m.uvBuffer,
            feedbackFBO ? m.feedbackNormalBuffer : m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            spotLightsDepthMapArray,
            indexLight,
            spotLightsDepthMapsFBO,
            feedbackFBO ? 0 : m.indexBuffer,
            feedbackFBO ? 0 : m.indexCount
        );
    }
}

void ShaderOGLShadowPass::renderMeshIntoDirectionalLightTexture(Mesh3D *o, bool useFeedbackFBO, const DirLightOpenGL& light) const
{
    for (const auto& m: o->getMeshData()) {
        if (!m.visibleInFrustum) continue;
        renderIntoDirectionalLightTexture(
            o,
            light,
            useFeedbackFBO ? m.feedbackBuffer : m.vertexBuffer,
            m.uvBuffer,
            useFeedbackFBO ? m.feedbackNormalBuffer : m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            directionalLightDepthMapFBO,
            useFeedbackFBO ? 0 : m.indexBuffer,
            useFeedbackFBO ? 0 : m.indexCount
        );
    }
}

void ShaderOGLShadowPass::renderIntoDirectionalLightTexture(
    Object3D* o,
    const DirLightOpenGL& light,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int size,
    GLuint fbo,
    GLuint indexBuffer,
    GLsizei indexCount
) const
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    auto shaderRender = Components::get()->Render()->getShaders()->shaderOGLRender;
    setMat4Uniform(matrixViewUniform, shaderRender->getDirectionalLightMatrix(light));
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    DrawMeshGeometry(GL_TRIANGLES, indexBuffer, indexCount, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLShadowPass::renderIntoArrayDepthTextures(
    Object3D* o,
    LightSpot* light,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    GLuint shadowMapArrayTex,
    int layer,
    GLuint fbo,
    GLuint indexBuffer,
    GLsizei indexCount
) const
{
    if (light == nullptr) {
        LOG_MESSAGE("ShaderShadowPass Error: Empty LightPoint3D!!");
        return;
    }

    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);
    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    setMat4Uniform(matrixViewUniform, light->getLightSpaceMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapArrayTex, 0, layer);
    DrawMeshGeometry(GL_TRIANGLES, indexBuffer, indexCount, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

// Fase 4 (instancing, segunda rebanada): cuerpo antes duplicado byte a byte entre
// renderSceneDirectionalLight/renderSceneSpotLight. Cull por AABB contra el VP de la luz igual
// que antes (Fase 1.3) -- ocurre ANTES de agrupar, así que un run nunca mezcla un submesh que
// debería estar culled. Después agrupa submeshes consecutivos (incluyendo entre casters
// distintos) que comparten toda la geometría salvo la matriz de modelo, mismo patrón que
// ComponentRender::FlushOpaqueQueue. Run de 1 -> draw normal. Run de 2+ -> instanciado.
void ShaderOGLShadowPass::drawCastersInstanced(
    const std::vector<Mesh3D*>& casters,
    const std::vector<std::vector<AABB3D>>& casterSubmeshWorldAabbs,
    const glm::mat4& lightVP
) const {
    std::vector<ShadowQueueEntry> queue;

    for (size_t i = 0; i < casters.size(); i++) {
        auto* mesh = casters[i];
        const auto& submeshAabbs = casterSubmeshWorldAabbs[i];
        const bool feedbackFBO = dynamic_cast<Mesh3DAnimation*>(mesh) != nullptr;
        const glm::mat4 model = mesh->getModelMatrix();
        const auto& meshData = mesh->getMeshData();
        for (size_t j = 0; j < meshData.size(); j++) {
            AABB3D worldAabb = submeshAabbs[j];
            if (!Frustum::isAABBVisibleInVP(&worldAabb, lightVP)) continue;
            const auto& m = meshData[j];

            ShadowQueueEntry entry;
            entry.vertexBuffer = feedbackFBO ? m.feedbackBuffer : m.vertexBuffer;
            entry.uvBuffer = m.uvBuffer;
            entry.normalBuffer = feedbackFBO ? m.feedbackNormalBuffer : m.normalBuffer;
            entry.indexBuffer = feedbackFBO ? 0 : m.indexBuffer;
            entry.indexCount = feedbackFBO ? 0 : m.indexCount;
            entry.vertexCount = static_cast<int>(m.vertices.size());
            entry.model = model;
            queue.push_back(entry);
        }
    }

    if (queue.empty()) return;

    std::sort(queue.begin(), queue.end(), [](const ShadowQueueEntry &a, const ShadowQueueEntry &b) {
        return a.vertexBuffer < b.vertexBuffer;
    });

    auto sameBatch = [](const ShadowQueueEntry &a, const ShadowQueueEntry &b) {
        return a.vertexBuffer == b.vertexBuffer && a.uvBuffer == b.uvBuffer &&
               a.normalBuffer == b.normalBuffer && a.indexBuffer == b.indexBuffer &&
               a.indexCount == b.indexCount;
    };

    size_t i = 0;
    while (i < queue.size()) {
        size_t j = i + 1;
        while (j < queue.size() && sameBatch(queue[i], queue[j])) ++j;

        const auto& first = queue[i];
        if (j - i == 1) {
            setBoolUniform(useInstancingUniform, false);
            setMat4Uniform(matrixModelUniform, first.model);
            setVAOAttributes(first.vertexBuffer, first.uvBuffer, first.normalBuffer);
            DrawMeshGeometry(GL_TRIANGLES, first.indexBuffer, first.indexCount, first.vertexCount);
        } else {
            std::vector<glm::mat4> models;
            models.reserve(j - i);
            for (size_t k = i; k < j; k++) models.push_back(queue[k].model);

            setBoolUniform(useInstancingUniform, true);
            setVAOAttributes(first.vertexBuffer, first.uvBuffer, first.normalBuffer);

            glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                static_cast<GLsizeiptr>(models.size() * sizeof(glm::mat4)),
                models.data(),
                GL_DYNAMIC_DRAW
            );

            DrawMeshGeometryInstanced(
                GL_TRIANGLES, first.indexBuffer, first.indexCount, first.vertexCount,
                static_cast<GLsizei>(models.size())
            );
        }
        i = j;
    }
}

void ShaderOGLShadowPass::renderSceneDirectionalLight(
    const std::vector<Mesh3D*>& casters,
    const std::vector<std::vector<AABB3D>>& casterSubmeshWorldAabbs,
    const DirLightOpenGL& light
) const {
    auto render = Components::get()->Render();
    const int res = Config::get()->SHADOW_MAP_RESOLUTION;
    glViewport(0, 0, res, res);
    render->ChangeOpenGLFramebuffer(directionalLightDepthMapFBO);
    render->ChangeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    auto shaderRender = render->getShaders()->shaderOGLRender;
    glm::mat4 lightVP = shaderRender->getDirectionalLightMatrix(light);
    setMat4Uniform(matrixViewUniform, lightVP);

    drawCastersInstanced(casters, casterSubmeshWorldAabbs, lightVP);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    render->ChangeOpenGLFramebuffer(0);

    auto win = Components::get()->Window();
    glViewport(0, 0, win->getWidthRender(), win->getHeightRender());
}

void ShaderOGLShadowPass::renderSceneSpotLight(
    const std::vector<Mesh3D*>& casters,
    const std::vector<std::vector<AABB3D>>& casterSubmeshWorldAabbs,
    LightSpot* light,
    int layerIndex
) const {
    if (light == nullptr) return;

    auto render = Components::get()->Render();
    const int res = Config::get()->SHADOW_MAP_RESOLUTION;
    glViewport(0, 0, res, res);
    render->ChangeOpenGLFramebuffer(spotLightsDepthMapsFBO);
    render->ChangeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, spotLightsDepthMapArray, 0, layerIndex);
    glm::mat4 lightVP = light->getLightSpaceMatrix();
    setMat4Uniform(matrixViewUniform, lightVP);

    drawCastersInstanced(casters, casterSubmeshWorldAabbs, lightVP);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    render->ChangeOpenGLFramebuffer(0);

    auto win = Components::get()->Window();
    glViewport(0, 0, win->getWidthRender(), win->getHeightRender());
}

GLuint ShaderOGLShadowPass::getSpotLightsDepthMapsFBO() const
{
    return spotLightsDepthMapsFBO;
}

GLuint ShaderOGLShadowPass::getDirectionalLightDepthMapFBO() const
{
    return directionalLightDepthMapFBO;
}

void ShaderOGLShadowPass::Destroy()
{
    ResetFramebuffers();
}

void ShaderOGLShadowPass::setupFBOSpotLights()
{
    if (spotLightsDepthMapsFBO != 0) {
        glDeleteFramebuffers(1, &spotLightsDepthMapsFBO);
    }

    glGenFramebuffers(1, &spotLightsDepthMapsFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, spotLightsDepthMapsFBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(spotLightsDepthMapsFBO);
    // Para una textura tipo array, usamos glFramebufferTextureLayer en lugar de glFramebufferTexture2D
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, spotLightsDepthMapArray, 0, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
}

void ShaderOGLShadowPass::setupFBODirectionalLight()
{
    if (directionalLightDepthMapFBO != 0) {
        glDeleteFramebuffers(1, &directionalLightDepthMapFBO);
    }

    glGenFramebuffers(1, &directionalLightDepthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, directionalLightDepthMapFBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(directionalLightDepthMapFBO);

    // Para una textura normal, usamos glFramebufferTexture2D en lugar de glFramebufferTextureLayer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalLightDepthTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
}

GLuint ShaderOGLShadowPass::getDirectionalLightDepthTexture() const
{
    return directionalLightDepthTexture;
}

void ShaderOGLShadowPass::createDirectionalLightDepthTexture()
{
    if (directionalLightDepthTexture != 0) {
        glDeleteTextures(1, &directionalLightDepthTexture);
    }

    int res = Config::get()->SHADOW_MAP_RESOLUTION;

    glGenTextures(1, &directionalLightDepthTexture);
    glBindTexture(GL_TEXTURE_2D, directionalLightDepthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, res, res, 0,GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void ShaderOGLShadowPass::clearDirectionalLightDepthTexture() const
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(directionalLightDepthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShaderOGLShadowPass::ResetFramebuffers()
{
    createSpotLightsDepthTextures(MAX_SHADOW_CASTERS);
    setupFBOSpotLights();
    createDirectionalLightDepthTexture();
    setupFBODirectionalLight();
}

GLuint ShaderOGLShadowPass::getSpotLightsShadowMapArrayTextures() const
{
    return spotLightsDepthMapArray;
}

void ShaderOGLShadowPass::createSpotLightsDepthTextures(int numLights)
{
    LOG_MESSAGE("[ShaderOGLShadowPass] Allocating shadow map array for %d shadow casters", numLights);

    if (spotLightsDepthMapArray != 0) {
        glDeleteTextures(1, &spotLightsDepthMapArray);
        spotLightsDepthMapArray = 0;
    }

    int res = Config::get()->SHADOW_MAP_RESOLUTION;

    glGenTextures(1, &spotLightsDepthMapArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, spotLightsDepthMapArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, res, res, numLights,0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
}
