//
// Created by Eduardo on 16/12/2025.
//

#include "PostProcessingManager.h"
#include <algorithm>
#include <iostream>

#include "../Components/Components.h"
#include "../OpenGL/Nodes/ShaderNodesPostProcessing.h"

PostProcessingManager::PostProcessingManager()
    :
      postProcessingShaders(Components::get()->Render()->getSceneShaders())
    , pingFBO(0)
    , pingTexture(0)
    , pongFBO(0)
    , pongTexture(0)
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

    // Crear los dos framebuffers para ping-pong
    createFramebuffer(pingFBO, pingTexture, width, height);
    createFramebuffer(pongFBO, pongTexture, width, height);
}

void PostProcessingManager::createFramebuffer(GLuint& fbo, GLuint& texture, int width, int height)
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Adjuntar textura al framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Verificar que el framebuffer está completo
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Framebuffer no está completo!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessingManager::deleteFramebuffer(GLuint& fbo, GLuint& texture)
{
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
    if (width == currentWidth && height == currentHeight) {
        return;
    }

    currentWidth = width;
    currentHeight = height;

    // Recrear framebuffers con el nuevo tamaño
    deleteFramebuffer(pingFBO, pingTexture);
    deleteFramebuffer(pongFBO, pongTexture);

    createFramebuffer(pingFBO, pingTexture, width, height);
    createFramebuffer(pongFBO, pongTexture, width, height);
}

void PostProcessingManager::cleanup()
{
    // Limpiar framebuffers
    deleteFramebuffer(pingFBO, pingTexture);
    deleteFramebuffer(pongFBO, pongTexture);

    postProcessingShaders.clear();
}

ShaderBaseCustom* PostProcessingManager::getShader(int index) const
{
    if (index >= 0 && index < postProcessingShaders.size()) {
        return postProcessingShaders[index];
    }
    return nullptr;
}

void PostProcessingManager::processChain(GLuint inputTexture, GLuint outputFBO)
{
    if (postProcessingShaders.empty()) {
        return;
    }

    // Encontrar el índice del último shader HABILITADO
    int lastEnabledIndex = -1;
    for (int i = (int)postProcessingShaders.size() - 1; i >= 0; i--) {
        if (postProcessingShaders[i]->isEnabled()) {
            lastEnabledIndex = i;
            break;
        }
    }

    // Si no hay ningún shader habilitado, no hacer nada
    if (lastEnabledIndex < 0) {
        return;
    }

    GLuint currentInputTexture = inputTexture;
    bool writeToPing = true;

    for (size_t i = 0; i < postProcessingShaders.size(); i++) {
        auto currentShader = postProcessingShaders[i];

        if (!currentShader->isEnabled()) {
            continue;
        }

        // Si es un shader de nodos, actualizar texturas internas
        // Para la cadena: el color de entrada es la salida del shader anterior (currentInputTexture)
        // mientras que depth siempre es la textura de profundidad original de la escena
        if (auto* nodesShader = dynamic_cast<ShaderNodesPostProcessing*>(currentShader)) {
            nodesShader->UpdateInternalTextures(currentInputTexture, sceneDepthTexture);
        }

        // Determinar FBO y textura de salida
        GLuint currentOutputFBO;
        GLuint currentOutputTexture;

        bool isLastEnabled = (i == lastEnabledIndex);

        if (isLastEnabled) {
            // Último shader habilitado: escribir al FBO de salida final
            currentOutputFBO = outputFBO;
            currentOutputTexture = 0;
        } else {
            // Shaders intermedios: alternar ping-pong
            if (writeToPing) {
                currentOutputFBO = pingFBO;
                currentOutputTexture = pingTexture;
            } else {
                currentOutputFBO = pongFBO;
                currentOutputTexture = pongTexture;
            }
        }

        // Renderizar el shader
        currentShader->Render(currentOutputFBO, currentInputTexture);

        // La salida de este shader es la entrada del siguiente (si no es el último)
        if (!isLastEnabled) {
            currentInputTexture = currentOutputTexture;
            writeToPing = !writeToPing;
        }
    }
}

void PostProcessingManager::SetSceneTextures(GLuint colorTexture, GLuint depthTexture)
{
    sceneColorTexture = colorTexture;
    sceneDepthTexture = depthTexture;
}