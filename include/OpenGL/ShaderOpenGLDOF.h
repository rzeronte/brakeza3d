//
// Created by edu on 22/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLDOF_H
#define BRAKEZA3D_SHADEROPENGLDOF_H

#include "ShaderBaseOpenGL.h"
#include "ShaderBaseOpenGLQuad.h"

class ShaderOpenGLDOF : ShaderBaseOpenGL, public ShaderBaseOpenGLQuad {

    GLuint focalDistanceUniform;
    GLuint focalRangeUniform;
    GLuint blurRadiusUniform;
    GLuint intensityUniform;
    GLuint farPlaneUniform;

    GLuint depthTextureUniform;
    GLuint sceneTextureUniform;

    GLuint widthUniform;
    GLuint heightUniform;

public:
    ShaderOpenGLDOF();

    void render(GLuint sceneTexture, GLuint depthTexture);
    void createFramebuffer();

    GLuint resultFramebuffer;
    GLuint textureResult;

    float focalRange;
    float focalDistance;
    int blurRadius;
    float intensity;
    float farPlane;

    [[nodiscard]] GLuint getTextureResult() const;
    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLDOF_H
