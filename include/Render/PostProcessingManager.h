
#ifndef BRAKEZA3D_POSTPROCESSINGMANAGER_H
#define BRAKEZA3D_POSTPROCESSINGMANAGER_H

#pragma once

#include <vector>

#include "../OpenGL/Base/ShaderBaseCustom.h"

class PostProcessingManager {
private:
    std::vector<ShaderBaseCustom*> &postProcessingShaders;

    // Ping framebuffer
    GLuint pingFBO = 0;
    GLuint pingTexture = 0;

    // Pong framebuffer
    GLuint pongFBO = 0;
    GLuint pongTexture = 0;

    // Quad fullscreen
    int currentWidth;
    int currentHeight;

    GLuint sceneDepthTexture = 0;
    GLuint sceneColorTexture = 0;

    void createFramebuffer(GLuint& fbo, GLuint& texture, int width, int height);
    void deleteFramebuffer(GLuint& fbo, GLuint& texture);

public:
    PostProcessingManager(int width, int height);
    ~PostProcessingManager();

    void resize(int width, int height);
    void cleanup();

    [[nodiscard]] GLuint getPingFBO() const { return pingFBO; }
    [[nodiscard]] GLuint getPingTexture() const { return pingTexture; }
    [[nodiscard]] GLuint getPongFBO() const { return pongFBO; }
    [[nodiscard]] GLuint getPongTexture() const { return pongTexture;}

    void processChain(GLuint inputTexture, GLuint outputFBO);

    int getShaderCount() const { return postProcessingShaders.size(); }
    ShaderBaseCustom* getShader(int index) const;
    void SetSceneTextures(GLuint colorTexture, GLuint depthTexture);
    GLuint GetSceneDepthTexture() const { return sceneDepthTexture; }
    GLuint GetSceneColorTexture() const { return sceneColorTexture; }
};

#endif //BRAKEZA3D_POSTPROCESSINGMANAGER_H