//
// Created by Eduardo on 13/11/2025.
//

#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include "../../include/Config.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"

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
}

void ShaderOGLShadowPass::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    ResetFramebuffers();
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
            spotLightsDepthMapsFBO
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
            directionalLightDepthMapFBO
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
    GLuint fbo
) const
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    auto shaderRender = Components::get()->Render()->getShaders()->shaderOGLRender;
    setMat4Uniform(matrixViewUniform, shaderRender->getDirectionalLightMatrix(light));
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    glDrawArrays(GL_TRIANGLES, 0, size);

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
    GLuint fbo
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
    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLShadowPass::renderSceneDirectionalLight(
    const std::vector<Mesh3D*>& casters,
    const DirLightOpenGL& light
) const {
    auto render = Components::get()->Render();
    const int res = Config::get()->SHADOW_MAP_RESOLUTION;
    glViewport(0, 0, res, res);
    render->ChangeOpenGLFramebuffer(directionalLightDepthMapFBO);
    render->ChangeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    auto shaderRender = render->getShaders()->shaderOGLRender;
    setMat4Uniform(matrixViewUniform, shaderRender->getDirectionalLightMatrix(light));

    for (auto* mesh : casters) {
        const bool feedbackFBO = dynamic_cast<Mesh3DAnimation*>(mesh) != nullptr;
        setMat4Uniform(matrixModelUniform, mesh->getModelMatrix());
        for (const auto& m : mesh->getMeshData()) {
            setVAOAttributes(
                feedbackFBO ? m.feedbackBuffer : m.vertexBuffer,
                m.uvBuffer,
                feedbackFBO ? m.feedbackNormalBuffer : m.normalBuffer
            );
            glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(m.vertices.size()));
        }
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    render->ChangeOpenGLFramebuffer(0);

    auto win = Components::get()->Window();
    glViewport(0, 0, win->getWidthRender(), win->getHeightRender());
}

void ShaderOGLShadowPass::renderSceneSpotLight(
    const std::vector<Mesh3D*>& casters,
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
    setMat4Uniform(matrixViewUniform, light->getLightSpaceMatrix());

    for (auto* mesh : casters) {
        const bool feedbackFBO = dynamic_cast<Mesh3DAnimation*>(mesh) != nullptr;
        setMat4Uniform(matrixModelUniform, mesh->getModelMatrix());
        for (const auto& m : mesh->getMeshData()) {
            setVAOAttributes(
                feedbackFBO ? m.feedbackBuffer : m.vertexBuffer,
                m.uvBuffer,
                feedbackFBO ? m.feedbackNormalBuffer : m.normalBuffer
            );
            glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(m.vertices.size()));
        }
    }

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
    // Para una textura tipo array, usamos glFramebufferTextureLayer en lugar de glFramebufferTexture2D
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, spotLightsDepthMapArray, 0, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOGLShadowPass::setupFBODirectionalLight()
{
    if (directionalLightDepthMapFBO != 0) {
        glDeleteFramebuffers(1, &directionalLightDepthMapFBO);
    }

    glGenFramebuffers(1, &directionalLightDepthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, directionalLightDepthMapFBO);

    // Para una textura normal, usamos glFramebufferTexture2D en lugar de glFramebufferTextureLayer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalLightDepthTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
