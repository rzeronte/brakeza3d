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
    GLuint pingFBO = 0;
    GLuint pingTexture = 0;
    GLuint pingDepthRB = 0;

    GLuint pongFBO = 0;
    GLuint pongTexture = 0;
    GLuint pongDepthRB = 0;

    int width = 0;
    int height = 0;
    GLuint lastOutputTexture = 0;

    void createFramebuffer(GLuint& fbo, GLuint& tex, GLuint& depth, int w, int h) {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // Color texture - captures gAlbedoSpec (shader output location 2)
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

        // Map GBuffer layout: discard locations 0,1 (gPosition, gNormal),
        // route location 2 (gAlbedoSpec) to our single color attachment
        GLenum drawBuffers[3] = { GL_NONE, GL_NONE, GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(3, drawBuffers);

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

    void deleteFramebuffer(GLuint& fbo, GLuint& tex, GLuint& depth) {
        if (depth) { glDeleteRenderbuffers(1, &depth); depth = 0; }
        if (tex) { glDeleteTextures(1, &tex); tex = 0; }
        if (fbo) { glDeleteFramebuffers(1, &fbo); fbo = 0; }
    }

public:
    ~Mesh3DShaderChain() {
        deleteFramebuffer(pingFBO, pingTexture, pingDepthRB);
        deleteFramebuffer(pongFBO, pongTexture, pongDepthRB);
    }

    void Initialize(int screenWidth, int screenHeight) {
        width = screenWidth;
        height = screenHeight;

        createFramebuffer(pingFBO, pingTexture, pingDepthRB, width, height);
        createFramebuffer(pongFBO, pongTexture, pongDepthRB, width, height);
    }

    GLuint GetLastTempTexture() const {
        // Return the last used ping or pong texture
        return (pingTexture != 0) ? pingTexture : pongTexture;
    }

    GLuint GetLastOutputTexture() const {
        return lastOutputTexture;
    }

    bool isInitialized() const {
        return (pingFBO != 0 && pongFBO != 0);
    }

    void ProcessChain(const Mesh3D* mesh, const std::vector<ShaderBaseCustom*>& shaders, GLuint finalFBO);
};
