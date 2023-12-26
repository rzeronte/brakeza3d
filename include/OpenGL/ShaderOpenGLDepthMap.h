//
// Created by edu on 26/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLDEPTHMAP_H
#define BRAKEZA3D_SHADEROPENGLDEPTHMAP_H


#include "ShaderOpenGL.h"

class ShaderOpenGLDepthMap : public ShaderOpenGL {
    GLuint quadVAO;
    GLuint VBO;

    GLint modelMatrixUniform;
    GLint projectionMatrixUniform;
    GLint textureUniform;

    GLint intensityUniform;
    GLint farPlaneUniform;
public:
    ShaderOpenGLDepthMap();
    void render(GLuint textureID, GLuint framebuffer);
};


#endif //BRAKEZA3D_SHADEROPENGLDEPTHMAP_H
