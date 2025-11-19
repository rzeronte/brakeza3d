//
// Created by edu on 8/12/23.
//

#ifndef BRAKEZA3D_OPENGLSHADERTYPES_H
#define BRAKEZA3D_OPENGLSHADERTYPES_H


#include <glm/vec3.hpp>

struct Material {
    GLint diffuse;
    GLint specular;
    float shininess;
};

struct DirLightOpenGL {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLightOpenGL {
    glm::vec4 position;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float padding;
};

struct SpotLightOpenGL {
    glm::vec4 position;
    glm::vec4 direction;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
    float padding;
    float padding1;
    float padding2;
};

struct OpenGLPickingBuffer {
    GLuint FBO = 0;
    GLuint rbgTexture = 0;
    GLuint depthTexture = 0;

    [[nodiscard]] GLuint getFBO() const {
        return FBO;
    }
    [[nodiscard]] GLuint getRGBTexture() const {
        return rbgTexture;
    }
    [[nodiscard]] GLuint getDepthTexture() const {
        return depthTexture;
    }
};

struct OpenGLGBuffer {
    GLuint FBO = 0;
    GLuint gPosition = 0;
    GLuint gNormal = 0;
    GLuint gAlbedoSpec = 0;
    GLuint rboDepth = 0;

    [[nodiscard]] GLuint getFBO() const {
        return FBO;
    }
    [[nodiscard]] GLuint getPositions() const {
        return gPosition;
    }
    [[nodiscard]] GLuint getAlbedo() const {
        return gAlbedoSpec;
    }
    [[nodiscard]] GLuint getNormal() const {
        return gNormal;
    }
    [[nodiscard]] GLuint getDepth() const {
        return rboDepth;
    }
};

struct OpenGLGlobalFramebuffers
{
    GLuint globalFramebuffer;
    GLuint sceneFramebuffer;
    GLuint backgroundFramebuffer;
    GLuint foregroundFramebuffer;
    GLuint uiFramebuffer;
    GLuint postProcessingFramebuffer;

    GLuint globalTexture;
    GLuint sceneTexture;
    GLuint backgroundTexture;
    GLuint foregroundTexture;
    GLuint uiTexture;
    GLuint postProcessingTexture;

    GLuint depthTexture;
};

#endif //BRAKEZA3D_OPENGLSHADERTYPES_H
