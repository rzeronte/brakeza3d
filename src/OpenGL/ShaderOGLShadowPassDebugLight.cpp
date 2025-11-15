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

void ShaderOGLShadowPassDebugLight::render(GLuint shadowMapArrayTex, int layer, GLuint framebuffer)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();
    setInt("layer", layer);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArrayTex);

    drawQuad();

    glBindVertexArray(0);
}

void ShaderOGLShadowPassDebugLight::renderInternal(GLuint shadowMapArrayTex, int layer)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(internalFramebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();
    setInt("layer", layer);

    glBindFramebuffer(GL_FRAMEBUFFER, internalFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, internalTextures[layer], 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArrayTex);

    drawQuad();

    glBindVertexArray(0);
}

void ShaderOGLShadowPassDebugLight::destroy()
{
    clearInternalTextures();
}

void ShaderOGLShadowPassDebugLight::createFramebuffer()
{
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Restaurar framebuffer por defecto
}

GLuint ShaderOGLShadowPassDebugLight::getSceneFramebuffer() const {
    return internalFramebuffer;
}

GLuint ShaderOGLShadowPassDebugLight::getTexture() const {
    return sceneTexture;
}

void ShaderOGLShadowPassDebugLight::createTextures(int numLayers)
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
    auto window = ComponentsManager::get()->getComponentWindow();

    for (int i = 0; i < numLights; i++) {
        renderInternal(window->getShadowMapArrayTex(), i);
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