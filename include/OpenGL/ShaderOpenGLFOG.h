//
// Created by edu on 26/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLFOG_H
#define BRAKEZA3D_SHADEROPENGLFOG_H


#include "ShaderBaseOpenGL.h"
#include "../Misc/Color.h"
#include "ShaderBaseOpenGLQuad.h"

class ShaderOpenGLFOG : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad {
    GLuint resultFramebuffer;
    GLuint textureResult;

    GLuint intensityUniform;
    GLuint fogMaxDistUniform;
    GLuint fogMinDistUniform;
    GLuint fogColourUniform;
    GLuint farPlaneUniform;

    GLuint depthTextureUniform;
    GLuint sceneTextureUniform;

public:
    ShaderOpenGLFOG();

    void render(GLuint sceneTexture, GLuint depthTexture);
    void createFramebuffer();

    float fogMaxDist;
    float fogMinDist;
    float intensity;
    Color fogColor;

    [[nodiscard]] GLuint getTextureResult() const;
    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLFOG_H
