//
// Created by Eduardo on 13/11/2025.
//

#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOGLShadowPass::ShaderOGLShadowPass()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "ShadowPass.vs",
        EngineSetup::get()->SHADERS_FOLDER + "ShadowPass.fs",
        false
    ),
    VertexArrayID(0),
    spotLightsDepthMapsFBO(0),
    directionalLightDepthMapFBO(0),
    directionalLightDepthTexture(0)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixViewUniform = glGetUniformLocation(programID, "lightSpaceMatrix");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    resetFramebuffers();
}

void ShaderOGLShadowPass::renderMeshIntoArrayTextures(
    Mesh3D *o,
    SpotLight3D* light,
    GLuint depthArrayTextures,
    int indexLight,
    GLuint fb
) const {
    for (const auto& m: o->meshes) {
        renderIntoArrayDepthTextures(
            o,
            light,
            m.vertexBuffer,
            m.uvBuffer,
            m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            depthArrayTextures,
            indexLight,
            fb
        );
    }
}

void ShaderOGLShadowPass::renderMeshIntoDirectionalLightTexture(Mesh3D *o, const DirLightOpenGL& light, GLuint framebuffer) const
{
    for (const auto& m: o->meshes) {
        renderIntoDirectionalLightTexture(
            o,
            light,
            m.vertexBuffer,
            m.uvBuffer,
            m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            framebuffer
        );
    }
}




void ShaderOGLShadowPass::renderMeshAnimatedIntoArrayTextures(
    Mesh3DAnimation *o,
    SpotLight3D* light,
    GLuint depthArrayTextures,
    int indexLight,
    GLuint fb
) const {
    for (const auto& m: o->meshes) {
        renderIntoArrayDepthTextures(
            o,
            light,
            m.feedbackBuffer,
            m.uvBuffer,
            m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            depthArrayTextures,
            indexLight,
            fb
        );
    }
}

void ShaderOGLShadowPass::renderMeshAnimatedIntoDirectionalLightTexture(Mesh3DAnimation *o, const DirLightOpenGL& light, GLuint framebuffer) const
{
    for (const auto& m: o->meshes) {
        renderIntoDirectionalLightTexture(
            o,
            light,
            m.feedbackBuffer,
            m.uvBuffer,
            m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            framebuffer
        );
    }
}


void ShaderOGLShadowPass::renderIntoDirectionalLightTexture(
    Object3D* o,
    const DirLightOpenGL& light,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    GLuint framebuffer
) const {

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    auto shaderRender = ComponentsManager::get()->getComponentRender()->getShaderOGLRenderForward();
    setMat4Uniform(matrixViewUniform, shaderRender->getDirectionalLightMatrix(light));
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(0);
}

void ShaderOGLShadowPass::renderIntoArrayDepthTextures(
    Object3D* o,
    SpotLight3D* light,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    GLuint shadowMapArrayTex,
    int layer,
    GLuint framebuffer
) const
{
    if (light == nullptr) {
        Logging::Message("ShaderShadowPass Error: Empty LightPoint3D!!");
        return;
    }

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    setMat4Uniform(matrixViewUniform, light->getLightSpaceMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapArrayTex, 0, layer);

    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(0);
}

GLuint ShaderOGLShadowPass::getSpotLightsDepthMapsFBO() const
{
    return spotLightsDepthMapsFBO;
}

GLuint ShaderOGLShadowPass::getDirectionalLightDepthMapFBO() const
{
    return directionalLightDepthMapFBO;
}

void ShaderOGLShadowPass::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShaderOGLShadowPass::destroy()
{
    resetFramebuffers();
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

GLuint ShaderOGLShadowPass::getDirectionalLightDepthTexture() const {
    return directionalLightDepthTexture;
}

void ShaderOGLShadowPass::createDirectionalLightDepthTexture()
{
    if (directionalLightDepthTexture != 0) {
        glDeleteTextures(1, &directionalLightDepthTexture);
    }

    auto window = ComponentsManager::get()->getComponentWindow();

    int width = window->getWidthRender();
    int height = window->getHeightRender();

    glGenTextures(1, &directionalLightDepthTexture);
    glBindTexture(GL_TEXTURE_2D, directionalLightDepthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(directionalLightDepthMapFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, directionalLightDepthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShaderOGLShadowPass::resetFramebuffers()
{
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
    auto window = ComponentsManager::get()->getComponentWindow();

    glGenTextures(1, &spotLightsDepthMapArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, spotLightsDepthMapArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, window->getWidthRender(), window->getHeightRender(), numLights,0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
}
