
#ifndef BRAKEZA3D_POSTPROCESSINGMANAGER_H
#define BRAKEZA3D_POSTPROCESSINGMANAGER_H

#pragma once

#include <vector>

#include "../OpenGL/Base/ShaderBaseCustom.h"

class PostProcessingManager {
private:
    std::vector<ShaderBaseCustom*> &postProcessingShaders;

    // Ping framebuffer
    GLuint pingFBO;
    GLuint pingTexture;

    // Pong framebuffer
    GLuint pongFBO;
    GLuint pongTexture;

    // Quad fullscreen
    int currentWidth;
    int currentHeight;

    void createFramebuffer(GLuint& fbo, GLuint& texture, int width, int height);
    void deleteFramebuffer(GLuint& fbo, GLuint& texture);

public:
    PostProcessingManager();
    ~PostProcessingManager();

    void Initialize(int width, int height);
    void resize(int width, int height);
    void cleanup();

    [[nodiscard]] GLuint getPingFBO() const { return pingFBO; }
    [[nodiscard]] GLuint getPingTexture() const { return pingTexture; }
    [[nodiscard]] GLuint getPongFBO() const { return pongFBO; }
    [[nodiscard]] GLuint getPongTexture() const { return pongTexture;}

    void processChain(GLuint inputTexture, GLuint outputFBO);

    int getShaderCount() const { return postProcessingShaders.size(); }
    ShaderBaseCustom* getShader(int index) const;
};

#endif //BRAKEZA3D_POSTPROCESSINGMANAGER_H