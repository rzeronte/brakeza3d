//
// Created by edu on 22/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLDOF_H
#define BRAKEZA3D_SHADEROPENGLDOF_H

#include "ShaderOpenGL.h"
#include "ShaderQuadOpenGL.h"

class ShaderOpenGLDOF : ShaderOpenGL, public ShaderQuadOpenGL  {

    GLint focalDistanceUniform;
    GLint focalRangeUniform;
    GLint blurRadiusUniform;
    GLint intensityUniform;
    GLint farPlaneUniform;

    GLint depthTextureUniform;
    GLint sceneTextureUniform;

    GLint widthUniform;
    GLint heightUniform;

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

    GLuint getTextureResult() const;
    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLDOF_H
