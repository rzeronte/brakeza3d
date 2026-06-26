
#ifndef BRAKEZA3D_POSTPROCESSINGMANAGER_H
#define BRAKEZA3D_POSTPROCESSINGMANAGER_H

#pragma once

#include <vector>

#include "../OpenGL/Base/ShaderBaseCustom.h"
#include "../OpenGL/ShaderOGLImage.h"

class PostProcessingManager {
private:
    std::vector<ShaderBaseCustom*> &postProcessingShaders;

    // Per-shader framebuffers
    std::vector<GLuint> shaderFBOs;
    std::vector<GLuint> shaderTextures;
    std::vector<GLuint> shaderDepthRBOs;

    // Quad fullscreen
    int currentWidth;
    int currentHeight;

    GLuint sceneDepthTexture = 0;
    GLuint sceneColorTexture = 0;

    void createFramebuffer(GLuint& fbo, GLuint& texture, GLuint& rbo, int width, int height);
    void deleteFramebuffer(GLuint& fbo, GLuint& texture, GLuint& rbo);

public:
    PostProcessingManager();
    ~PostProcessingManager();

    void Initialize(int width, int height);
    void resize(int width, int height);
    void cleanup();

    void rebuildFBOs();

    [[nodiscard]] GLuint getShaderFBO(int index) const;
    [[nodiscard]] GLuint getShaderTexture(int index) const;

    void processChain(GLuint inputTexture, GLuint outputFBO);

    int getShaderCount() const { return postProcessingShaders.size(); }
    ShaderBaseCustom* getShader(int index) const;
    void SetSceneTextures(GLuint colorTexture, GLuint depthTexture);
    GLuint GetSceneDepthTexture() const { return sceneDepthTexture; }
    GLuint GetSceneColorTexture() const { return sceneColorTexture; }
};

#endif //BRAKEZA3D_POSTPROCESSINGMANAGER_H
