//
// Created by edu on 26/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLFOG_H
#define BRAKEZA3D_SHADEROPENGLFOG_H


#include "ShaderOpenGL.h"
#include "../Misc/Color.h"

class ShaderOpenGLFOG : public ShaderOpenGL {
    GLuint quadVAO, VBO;
    GLuint resultFramebuffer;
    GLuint textureResult;

    GLint modelMatrixUniform;
    GLint projectionMatrixUniform;

    GLint intensityUniform;
    GLint fogMaxDistUniform;
    GLint fogMinDistUniform;
    GLint fogColourUniform;
    GLint farPlaneUniform;


    GLint depthTextureUniform;
    GLint sceneTextureUniform;


public:
    ShaderOpenGLFOG();

    void render(GLuint sceneTexture, GLuint depthTexture);

    float fogMaxDist;
    float fogMinDist;
    float intensity;
    Color fogColor;

    GLuint getTextureResult() const;
};


#endif //BRAKEZA3D_SHADEROPENGLFOG_H
