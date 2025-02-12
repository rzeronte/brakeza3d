//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLTINT_H
#define BRAKEZA3D_SHADEROPENGLTINT_H


#include "../../include/OpenGL/ShaderOpenGL.h"
#include "../../include/OpenGL/ShaderQuadOpenGL.h"
#include "../../include/Misc/Color.h"

class ShaderOpenGLTint : public ShaderOpenGL, public ShaderQuadOpenGL {
    GLint colorUniform;
    GLint alphaUniform;

public:
    ShaderOpenGLTint();
    void render(Color c, float alpha, GLuint framebuffer);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTINT_H
