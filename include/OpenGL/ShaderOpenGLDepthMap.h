//
// Created by edu on 26/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLDEPTHMAP_H
#define BRAKEZA3D_SHADEROPENGLDEPTHMAP_H


#include "ShaderOpenGL.h"
#include "ShaderQuadOpenGL.h"

class ShaderOpenGLDepthMap : public ShaderOpenGL, public ShaderQuadOpenGL {

    GLint textureUniform;

    GLint intensityUniform;
    GLint farPlaneUniform;
    GLint nearPlaneUniform;
public:
    ShaderOpenGLDepthMap();
    void render(GLuint textureID, GLuint framebuffer);

    void destroy() override;

    float intensity;
    float farPlane;
    float nearPlane;
};


#endif //BRAKEZA3D_SHADEROPENGLDEPTHMAP_H
