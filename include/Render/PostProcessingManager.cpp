//
// Created by Eduardo on 16/12/2025.
//

#include "PostProcessingManager.h"
#include <algorithm>
#include <iostream>

#include "../Components/Components.h"
#include "../OpenGL/Nodes/ShaderNodesPostProcessing.h"
#include "../Misc/Logging.h"

PostProcessingManager::PostProcessingManager()
    :
      postProcessingShaders(Components::get()->Render()->getSceneShaders())
    , currentWidth(0)
    , currentHeight(0)
{
}

PostProcessingManager::~PostProcessingManager()
{
    cleanup();
}

void PostProcessingManager::Initialize(int width, int height)
{
    currentWidth = width;
    currentHeight = height;

    rebuildFBOs();
}

void PostProcessingManager::rebuildFBOs()
{
    for (size_t i = 0; i < shaderFBOs.size(); i++) {
        deleteFramebuffer(shaderFBOs[i], shaderTextures[i], shaderDepthRBOs[i]);
    }
    shaderFBOs.clear();
    shaderTextures.clear();
    shaderDepthRBOs.clear();

    shaderFBOs.resize(postProcessingShaders.size(), 0);
    shaderTextures.resize(postProcessingShaders.size(), 0);
    shaderDepthRBOs.resize(postProcessingShaders.size(), 0);

    for (size_t i = 0; i < postProcessingShaders.size(); i++) {
        createFramebuffer(shaderFBOs[i], shaderTextures[i], shaderDepthRBOs[i], currentWidth, currentHeight);
    }
}

void PostProcessingManager::createFramebuffer(GLuint& fbo, GLuint& texture, GLuint& rbo, int width, int height)
{
    // Generar framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Crear textura de color
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Adjuntar textura al framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    GLenum drawBuf = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &drawBuf);

    // Depth renderbuffer (never read, keeps some drivers happy)
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Verificar que el framebuffer está completo
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Framebuffer no está completo!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessingManager::deleteFramebuffer(GLuint& fbo, GLuint& texture, GLuint& rbo)
{
    if (rbo != 0) {
        glDeleteRenderbuffers(1, &rbo);
        rbo = 0;
    }

    if (texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }

    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
}

void PostProcessingManager::resize(int width, int height)
{
    currentWidth = width;
    currentHeight = height;

    for (size_t i = 0; i < shaderFBOs.size(); i++) {
        deleteFramebuffer(shaderFBOs[i], shaderTextures[i], shaderDepthRBOs[i]);
        createFramebuffer(shaderFBOs[i], shaderTextures[i], shaderDepthRBOs[i], width, height);
    }
}

void PostProcessingManager::cleanup()
{
    for (size_t i = 0; i < shaderFBOs.size(); i++) {
        deleteFramebuffer(shaderFBOs[i], shaderTextures[i], shaderDepthRBOs[i]);
    }
    shaderFBOs.clear();
    shaderTextures.clear();
    shaderDepthRBOs.clear();
}

ShaderBaseCustom* PostProcessingManager::getShader(int index) const
{
    if (index >= 0 && index < (int)postProcessingShaders.size()) {
        return postProcessingShaders[index];
    }
    return nullptr;
}

GLuint PostProcessingManager::getShaderFBO(int index) const
{
    if (index >= 0 && index < (int)shaderFBOs.size()) {
        return shaderFBOs[index];
    }
    return 0;
}

GLuint PostProcessingManager::getShaderTexture(int index) const
{
    if (index >= 0 && index < (int)shaderTextures.size()) {
        return shaderTextures[index];
    }
    return 0;
}

void PostProcessingManager::processChain(GLuint inputTexture, GLuint outputFBO)
{
    auto window = Components::get()->Window();
    int w = window->getWidthRender();
    int h = window->getHeightRender();

    if (w <= 0 || h <= 0) return;

    auto OGLImage = Components::get()->Render()->getShaders()->shaderOGLImage;
    // No shaders — blit scene directly to output
    if (postProcessingShaders.empty()) {
        OGLImage->renderTexture(inputTexture, 0, 0, w, h, w, h, 1, true, outputFBO);
        return;
    }

    // Ensure FBO count matches shader count (handles add/remove since last call)
    if (shaderFBOs.size() != postProcessingShaders.size()) {
        LOG_MESSAGE("[processChain] size mismatch, rebuilding...");
        if (currentWidth > 0 && currentHeight > 0) {
            rebuildFBOs();
        } else {
            LOG_MESSAGE("[processChain] skipping - curSize=%dx%d", currentWidth, currentHeight);
            return;
        }
    }

    // Set GL state once for the entire chain — fullscreen quad shaders don't need depth/cull/blend.
    glViewport(0, 0, w, h);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint currentInputTexture = inputTexture;

    for (size_t i = 0; i < postProcessingShaders.size(); i++) {
        auto currentShader = postProcessingShaders[i];

        if (!currentShader->isEnabled()) {
            continue;
        }

        GLuint currentOutputFBO = shaderFBOs[i];

        if (auto* nodesShader = dynamic_cast<ShaderNodesPostProcessing*>(currentShader)) {
            nodesShader->UpdateInternalTextures(currentInputTexture, sceneDepthTexture);
        }

        currentShader->Render(currentOutputFBO, currentInputTexture);

        currentInputTexture = shaderTextures[i];
    }

    // Final blit: copy last shader's output texture to the global output FBO
    OGLImage->renderTexture(currentInputTexture, 0, 0, w, h, w, h, 1, true, outputFBO);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void PostProcessingManager::SetSceneTextures(GLuint colorTexture, GLuint depthTexture)
{
    sceneColorTexture = colorTexture;
    sceneDepthTexture = depthTexture;
}
