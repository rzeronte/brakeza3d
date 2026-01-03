//
// Created by edu on 22/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLDOF_H
#define BRAKEZA3D_SHADEROPENGLDOF_H

#include "../Base/ShaderBaseOpenGL.h"
#include "../Base/ShaderBaseOpenGLQuad.h"

class ShaderOGLDOF : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{

    GLuint focalDistanceUniform = 0;
    GLuint focalRangeUniform = 0;
    GLuint blurRadiusUniform = 0;
    GLuint intensityUniform = 0;
    GLuint farPlaneUniform = 0;

    GLuint depthTextureUniform = 0;
    GLuint sceneTextureUniform = 0;

    GLuint widthUniform = 0;
    GLuint heightUniform = 0;

public:
    ShaderOGLDOF();
    void LoadUniforms() override;

    void PrepareMainThread() override;

    void render(GLuint sceneTexture, GLuint depthTexture);
    void CreateFramebuffer();

    GLuint resultFramebuffer = 0;
    GLuint textureResult = 0;

    float focalRange = 0;
    float focalDistance = 0;
    int blurRadius = 0;
    float intensity = 0;
    float farPlane = 0;

    [[nodiscard]] GLuint getTextureResult() const;
    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLDOF_H
