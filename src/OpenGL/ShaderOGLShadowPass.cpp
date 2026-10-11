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
        // Unidad animada con skinning en el shader (ver ShadowPass.vs): vertexBuffer es la geometría
        // compartida en pose de reposo y boneMatrices la pose de ESTA instancia en este frame
        // (Mesh3DAnimation::boneTransformCachePerMesh) -- así se agrupa con las del mismo modelo.
        // Geometría combinada del modelo (ShadowMergedGeometry): solo posiciones, sin uv/normal
        bool positionsOnly = false;
        bool isSkinned = false;
        GLuint vertexBoneDataBuffer = 0;
        int boneCount = 0;
        const std::vector<glm::mat4>* boneMatrices = nullptr;
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
    useSkinningUniform = glGetUniformLocation(programID, "useSkinning");
    bonesPerInstanceUniform = glGetUniformLocation(programID, "bonesPerInstance");
    boneMatricesSamplerUniform = glGetUniformLocation(programID, "boneMatrices");
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

    // TBO de matrices de huesos (unidades animadas instanciadas): glTexBuffer una sola vez, el
    // contenido se respecifica con glBufferData en cada lote -- igual que ShaderOGLRenderDeferred.
    glGenBuffers(1, &boneMatrixBuffer);
    glGenTextures(1, &boneMatrixTexture);
    glBindTexture(GL_TEXTURE_BUFFER, boneMatrixTexture);
    glBindBuffer(GL_TEXTURE_BUFFER, boneMatrixBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, boneMatrixBuffer);
    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
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
    setBoolUniform(useInstancingUniform, false);   // los uniforms persisten: fijar el modo
    setBoolUniform(useSkinningUniform, false);

    DrawMeshGeometry(GL_TRIANGLES, indexBuffer, indexCount, size, "ShadowPass:127");

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
    setBoolUniform(useInstancingUniform, false);   // los uniforms persisten: fijar el modo
    setBoolUniform(useSkinningUniform, false);

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapArrayTex, 0, layer);
    DrawMeshGeometry(GL_TRIANGLES, indexBuffer, indexCount, size, "ShadowPass:165");

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
        auto* anim = dynamic_cast<Mesh3DAnimation*>(mesh);
        const glm::mat4 model = mesh->getModelMatrix();

        // Modelo estático con geometría combinada: una entrada por instancia (el modelo entero); las
        // instancias del mismo modelo se agrupan en una llamada instanciada más abajo. Cull con la
        // caja del modelo entero (RunShadowPass no calcula cajas por submesh para estos casters).
        const ShadowMergedGeometry* merged = (anim == nullptr && Components::get()->Render()->isShadowMergeEnabled())
            ? mesh->getShadowMerged() : nullptr;
        if (merged != nullptr) {
            AABB3D localAabb;
            localAabb.min = Vertex3D(merged->localMin.x, merged->localMin.y, merged->localMin.z);
            localAabb.max = Vertex3D(merged->localMax.x, merged->localMax.y, merged->localMax.z);
            localAabb.updateVertices();
            AABB3D worldAabb;
            for (int c = 0; c < 8; c++) {
                glm::vec4 wp = model * glm::vec4(localAabb.vertices[c].toGLM(), 1.0f);
                worldAabb.vertices[c] = Vertex3D(wp.x / wp.w, wp.y / wp.w, wp.z / wp.w);
            }
            if (!Frustum::isAABBVisibleInVP(&worldAabb, lightVP)) continue;
            ShadowQueueEntry entry;
            entry.vertexBuffer = merged->buffer;
            entry.vertexCount = merged->vertexCount;
            entry.model = model;
            entry.positionsOnly = true;
            queue.push_back(entry);
            continue;
        }

        const auto& meshData = mesh->getMeshData();
        for (size_t j = 0; j < meshData.size(); j++) {
            AABB3D worldAabb = submeshAabbs[j];
            if (!Frustum::isAABBVisibleInVP(&worldAabb, lightVP)) continue;
            const auto& m = meshData[j];

            ShadowQueueEntry entry;
            entry.uvBuffer = m.uvBuffer;
            entry.vertexCount = static_cast<int>(m.vertices.size());
            entry.model = model;
            // Animada con pose calculada este frame: skinning en el shader sobre la geometría
            // compartida (se agrupa con las demás del mismo modelo). Sin pose: ver más abajo.
            const std::vector<glm::mat4>* bones = nullptr;
            if (anim != nullptr && m.vertexBoneDataBuffer != 0 && j < anim->getBoneTransformCacheCount()) {
                const auto& cache = anim->getBoneTransformCache(j);
                if (!cache.empty()) bones = &cache;
            }
            if (bones != nullptr) {
                entry.vertexBuffer = m.vertexBuffer;
                entry.normalBuffer = m.normalBuffer;
                entry.isSkinned = true;
                entry.vertexBoneDataBuffer = m.vertexBoneDataBuffer;
                entry.boneCount = static_cast<int>(bones->size());
                entry.boneMatrices = bones;
            } else {
                // Sin pose: geometría original (submesh sin huesos = igual que su pose; objeto aún
                // sin actualizar = pose de reposo). No feedbackBuffer: solo se hornea si algún
                // consumidor lo pide (Mesh3DAnimation::NeedsBakedPose), podría estar desfasado.
                // Las animadas nunca usan el EBO (siempre glDrawArrays), igual que en el G-Buffer.
                entry.vertexBuffer = m.vertexBuffer;
                entry.normalBuffer = m.normalBuffer;
                entry.indexBuffer = anim != nullptr ? 0 : m.indexBuffer;
                entry.indexCount = anim != nullptr ? 0 : m.indexCount;
            }
            queue.push_back(entry);
        }
    }

    if (queue.empty()) return;

    std::sort(queue.begin(), queue.end(), [](const ShadowQueueEntry &a, const ShadowQueueEntry &b) {
        if (a.vertexBuffer != b.vertexBuffer) return a.vertexBuffer < b.vertexBuffer;
        return a.isSkinned < b.isSkinned;
    });

    auto sameBatch = [](const ShadowQueueEntry &a, const ShadowQueueEntry &b) {
        return a.vertexBuffer == b.vertexBuffer && a.uvBuffer == b.uvBuffer &&
               a.normalBuffer == b.normalBuffer && a.indexBuffer == b.indexBuffer &&
               a.indexCount == b.indexCount && a.isSkinned == b.isSkinned &&
               a.vertexBoneDataBuffer == b.vertexBoneDataBuffer && a.boneCount == b.boneCount &&
               a.positionsOnly == b.positionsOnly;
    };

    // Geometría combinada: solo hay buffer de posiciones (atributo 0); el shader de sombras no lee
    // normal ni uv, así que se desactivan en vez de apuntarlas a un buffer inexistente.
    auto bindGeometry = [this](const ShadowQueueEntry &e) {
        if (!e.positionsOnly) {
            setVAOAttributes(e.vertexBuffer, e.uvBuffer, e.normalBuffer);
            return;
        }
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, e.vertexBuffer);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    };

    // Los uniforms persisten entre draws (y entre frames): el modo se fija en cada rama.
    setBoolUniform(useSkinningUniform, false);

    size_t i = 0;
    while (i < queue.size()) {
        size_t j = i + 1;
        while (j < queue.size() && sameBatch(queue[i], queue[j])) ++j;

        const auto& first = queue[i];
        if (first.isSkinned) {
            // Siempre instanciado, también con un solo elemento (gl_InstanceID = 0), igual que el
            // G-Buffer (ComponentRender::DrawRenderQueue).
            std::vector<glm::mat4> models;
            std::vector<glm::mat4> allBoneMatrices;
            models.reserve(j - i);
            allBoneMatrices.reserve((j - i) * first.boneCount);
            for (size_t k = i; k < j; k++) {
                models.push_back(queue[k].model);
                const auto& b = *queue[k].boneMatrices;
                allBoneMatrices.insert(allBoneMatrices.end(), b.begin(), b.end());
            }

            setBoolUniform(useInstancingUniform, true);
            setBoolUniform(useSkinningUniform, true);
            setIntUniform(bonesPerInstanceUniform, first.boneCount);
            setIntUniform(boneMatricesSamplerUniform, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_BUFFER, boneMatrixTexture);

            setVAOAttributes(first.vertexBuffer, first.uvBuffer, first.normalBuffer);
            setVAOBoneAttributes(first.vertexBoneDataBuffer);

            glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(models.size() * sizeof(glm::mat4)),
                models.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_TEXTURE_BUFFER, boneMatrixBuffer);
            glBufferData(GL_TEXTURE_BUFFER, static_cast<GLsizeiptr>(allBoneMatrices.size() * sizeof(glm::mat4)),
                allBoneMatrices.data(), GL_DYNAMIC_DRAW);
            glBindBuffer(GL_TEXTURE_BUFFER, 0);

            DrawMeshGeometryInstanced(GL_TRIANGLES, 0, 0, first.vertexCount,
                static_cast<GLsizei>(models.size()), "ShadowPass skinned");

            glDisableVertexAttribArray(7);
            glDisableVertexAttribArray(8);
            glBindTexture(GL_TEXTURE_BUFFER, 0);
            setBoolUniform(useSkinningUniform, false);
        } else if (j - i == 1) {
            setBoolUniform(useInstancingUniform, false);
            setMat4Uniform(matrixModelUniform, first.model);
            bindGeometry(first);
            DrawMeshGeometry(GL_TRIANGLES, first.indexBuffer, first.indexCount, first.vertexCount,
                first.positionsOnly ? "ShadowPass merged" : "ShadowPass:232");
        } else {
            std::vector<glm::mat4> models;
            models.reserve(j - i);
            for (size_t k = i; k < j; k++) models.push_back(queue[k].model);

            setBoolUniform(useInstancingUniform, true);
            bindGeometry(first);

            glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                static_cast<GLsizeiptr>(models.size() * sizeof(glm::mat4)),
                models.data(),
                GL_DYNAMIC_DRAW
            );

            DrawMeshGeometryInstanced(
                GL_TRIANGLES, first.indexBuffer, first.indexCount, first.vertexCount,
                static_cast<GLsizei>(models.size()), first.positionsOnly ? "ShadowPass merged instanced" : "ShadowPass:249"
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
