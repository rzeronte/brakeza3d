//
// Created by edu on 26/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLDEPTHMAP_H
#define BRAKEZA3D_SHADEROPENGLDEPTHMAP_H


#include "ShaderBaseOpenGL.h"
#include "ShaderBaseOpenGLQuad.h"

class ShaderOpenGLDepthMap : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad {

    GLint textureUniform;

    GLint intensityUniform;
    GLint nearUniform;
    GLint farUniform;

public:
    ShaderOpenGLDepthMap();
    void render(GLuint textureID, GLuint framebuffer);

    void destroy() override;

    float intensity;
    float farPlane;
    float nearPlane;
};


#endif //BRAKEZA3D_SHADEROPENGLDEPTHMAP_H
