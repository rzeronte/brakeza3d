//
// Created by Eduardo on 14/11/2025.
//

#include "../../include/OpenGL/ShaderOGLShadowPassDebugLight.h"
#include "../../include/ComponentsManager.h"

ShaderOGLShadowPassDebugLight::ShaderOGLShadowPassDebugLight()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "ShadowPassDebugLight.vs",
        EngineSetup::get()->SHADERS_FOLDER + "ShadowPassDebugLight.fs",
        false
    )
{
    setupQuadUniforms(programID);
    createFramebuffer();
}

void ShaderOGLShadowPassDebugLight::renderInternalToTexture()
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(internalFramebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    glBindFramebuffer(GL_FRAMEBUFFER, internalFramebuffer);

    auto shaderShadowPass = ComponentsManager::get()->getComponentRender()->getShaderOGLShadowPass();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shaderShadowPass->getDirectionalLightDepthTexture());

    drawQuad();

    glBindVertexArray(0);
}

void ShaderOGLShadowPassDebugLight::renderInternalFromArrayTextures(GLuint depthTexture, int layer)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(internalFramebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    glBindFramebuffer(GL_FRAMEBUFFER, internalFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, internalTextures[layer], 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    drawQuad();

    glBindVertexArray(0);
}

void ShaderOGLShadowPassDebugLight::destroy()
{
    clearInternalTextures();
}

void ShaderOGLShadowPassDebugLight::createFramebuffer()
{
    if (internalFramebuffer != 0) {
        glDeleteFramebuffers(1, &internalFramebuffer);
        glDeleteTextures(1, &sceneTexture);
    }

    glGenFramebuffers(1, &internalFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, internalFramebuffer);
    auto renderer = ComponentsManager::get()->getComponentWindow()->getRenderer();

    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);

    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Framebuffer no está completo!" << std::endl;
    } else {
        Logging::Message("ShadowPassDebugLight: Framebuffer created successful!");
    }
}

GLuint ShaderOGLShadowPassDebugLight::getSceneFramebuffer() const {
    return internalFramebuffer;
}

GLuint ShaderOGLShadowPassDebugLight::getSceneTexture() const {
    return sceneTexture;
}

void ShaderOGLShadowPassDebugLight::createArrayTextures(int numLayers)
{
    if (!internalTextures.empty()) {
        glDeleteTextures(static_cast<int>(internalTextures.size()), internalTextures.data());
        internalTextures.clear();
    }

    auto renderer = ComponentsManager::get()->getComponentWindow()->getRenderer();
    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);

    internalTextures.resize(numLayers);
    glGenTextures(numLayers, internalTextures.data());

    for (int i = 0; i < numLayers; i++) {
        glBindTexture(GL_TEXTURE_2D, internalTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    //Logging::Message("ShadowPassDebugLight: creating %d textures", numLayers);
}

GLuint ShaderOGLShadowPassDebugLight::getInternalTexture(int layer) const
{
    if (layer < 0 || layer >= static_cast<int>(internalTextures.size())) {
        std::cerr << "ERROR: ShadowPassDebugLight - Capa de textura " << layer
                  << " fuera de rango (0-" << (internalTextures.size()-1) << ")" << std::endl;
        return 0;
    }
    return internalTextures[layer];
}

void ShaderOGLShadowPassDebugLight::updateDebugTextures(int numLights)
{
    auto arrayTextures = ComponentsManager::get()->getComponentRender()->getSpotLightsShadowMapArrayTextures();

    renderInternalToTexture();

    for (int i = 0; i < numLights; i++) {
        auto depthTexture = this->extractLayerFromArray(arrayTextures, i);
        renderInternalFromArrayTextures(depthTexture, i);
        glDeleteTextures(1, &depthTexture);
    }
}

void ShaderOGLShadowPassDebugLight::clearInternalTextures()
{
    if (internalTextures.empty()) {
        return;
    }

    // Eliminar las texturas de OpenGL
    glDeleteTextures(internalTextures.size(), internalTextures.data());

    // Limpiar el vector
    internalTextures.clear();
}

GLuint ShaderOGLShadowPassDebugLight::extractLayerFromArray(GLuint arrayTexture, int layer)
{
    auto window = ComponentsManager::get()->getComponentWindow();

    int width, height;
    SDL_GetRendererOutputSize(window->getRenderer(), &width, &height);

    // Crear textura 2D destino
    GLuint texture2D;
    glGenTextures(1, &texture2D);
    glBindTexture(GL_TEXTURE_2D, texture2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Crear dos framebuffers separados
    GLuint fboRead, fboDraw;
    glGenFramebuffers(1, &fboRead);
    glGenFramebuffers(1, &fboDraw);

    // Configurar READ framebuffer (fuente: capa del array)
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboRead);
    glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, arrayTexture, 0, layer);

    // Configurar DRAW framebuffer (destino: textura 2D)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboDraw);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture2D, 0);

    // Copiar usando glBlitFramebuffer
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // Limpiar
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fboRead);
    glDeleteFramebuffers(1, &fboDraw);

    return texture2D;
}