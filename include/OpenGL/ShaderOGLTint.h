//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLTINT_H
#define BRAKEZA3D_SHADEROPENGLTINT_H


#include "../../include/OpenGL/ShaderBaseOpenGL.h"
#include "../../include/OpenGL/ShaderBaseOpenGLQuad.h"
#include "../../include/Misc/Color.h"

class ShaderOGLTint : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad {
    GLuint colorUniform;
    GLuint alphaUniform;

public:
    ShaderOGLTint();
    void render(Color c, float alpha, GLuint fbo);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTINT_H
