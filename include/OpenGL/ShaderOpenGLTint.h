//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLTINT_H
#define BRAKEZA3D_SHADEROPENGLTINT_H


#include "../../include/OpenGL/ShaderBaseOpenGL.h"
#include "../../include/OpenGL/ShaderBaseOpenGLQuad.h"
#include "../../include/Misc/Color.h"

class ShaderOpenGLTint : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad {
    GLint colorUniform;
    GLint alphaUniform;

public:
    ShaderOpenGLTint();
    void render(Color c, float alpha, GLuint framebuffer);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTINT_H
