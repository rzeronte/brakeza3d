#pragma once
#include <vector>
#include <iostream>
#include <GL/glew.h>

// Forward declarations para evitar dependencias circulares
class Mesh3D;
class ShaderBaseCustom;
class ShaderNodesMesh3D;
class Components;
class ShaderNodeEditorManager;

class Mesh3DShaderChain {
private:
    // Ping FBO - mirrors GBuffer layout (3 color attachments)
    GLuint pingFBO = 0;
    GLuint pingPositionTex = 0;  // attachment 0 - gPosition (discarded)
    GLuint pingNormalTex = 0;    // attachment 1 - gNormal (discarded)
    GLuint pingAlbedoTex = 0;    // attachment 2 - gAlbedoSpec (chain output)
    GLuint pingDepthRB = 0;

    // Pong FBO - mirrors GBuffer layout (3 color attachments)
    GLuint pongFBO = 0;
    GLuint pongPositionTex = 0;
    GLuint pongNormalTex = 0;
    GLuint pongAlbedoTex = 0;
    GLuint pongDepthRB = 0;

    int width = 0;
    int height = 0;
    GLuint lastOutputTexture = 0;

    void createFramebuffer(GLuint& fbo,
                           GLuint& posTex, GLuint& normTex, GLuint& albedoTex,
                           GLuint& depth, int w, int h)
    {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // Attachment 0 - positions (same as GBuffer layout)
        glGenTextures(1, &posTex);
        glBindTexture(GL_TEXTURE_2D, posTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);

        // Attachment 1 - normals (same as GBuffer layout)
        glGenTextures(1, &normTex);
        glBindTexture(GL_TEXTURE_2D, normTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normTex, 0);

        // Attachment 2 - albedo (this is the chain output we care about)
        glGenTextures(1, &albedoTex);
        glBindTexture(GL_TEXTURE_2D, albedoTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedoTex, 0);

        // Standard draw buffers matching GBuffer layout
        GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // Depth renderbuffer
        glGenRenderbuffers(1, &depth);
        glBindRenderbuffer(GL_RENDERBUFFER, depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR: Mesh3DShaderChain framebuffer not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void deleteFramebuffer(GLuint& fbo,
                           GLuint& posTex, GLuint& normTex, GLuint& albedoTex,
                           GLuint& depth)
    {
        if (depth) { glDeleteRenderbuffers(1, &depth); depth = 0; }
        if (albedoTex) { glDeleteTextures(1, &albedoTex); albedoTex = 0; }
        if (normTex) { glDeleteTextures(1, &normTex); normTex = 0; }
        if (posTex) { glDeleteTextures(1, &posTex); posTex = 0; }
        if (fbo) { glDeleteFramebuffers(1, &fbo); fbo = 0; }
    }

public:
    ~Mesh3DShaderChain() {
        deleteFramebuffer(pingFBO, pingPositionTex, pingNormalTex, pingAlbedoTex, pingDepthRB);
        deleteFramebuffer(pongFBO, pongPositionTex, pongNormalTex, pongAlbedoTex, pongDepthRB);
    }

    void Initialize(int screenWidth, int screenHeight) {
        width = screenWidth;
        height = screenHeight;

        createFramebuffer(pingFBO, pingPositionTex, pingNormalTex, pingAlbedoTex, pingDepthRB, width, height);
        createFramebuffer(pongFBO, pongPositionTex, pongNormalTex, pongAlbedoTex, pongDepthRB, width, height);
    }

    GLuint GetLastOutputTexture() const {
        return lastOutputTexture;
    }

    bool isInitialized() const {
        return (pingFBO != 0 && pongFBO != 0);
    }

    // Getters for FBOs and albedo textures (chain output)
    GLuint getPingFBO() const { return pingFBO; }
    GLuint getPongFBO() const { return pongFBO; }
    GLuint getPingAlbedoTexture() const { return pingAlbedoTex; }
    GLuint getPongAlbedoTexture() const { return pongAlbedoTex; }

    void ProcessChain(const Mesh3D* mesh, const std::vector<ShaderBaseCustom*>& shaders, GLuint finalFBO);
};
