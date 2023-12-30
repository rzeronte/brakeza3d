//
// Created by edu on 30/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLIMAGEOFFSET_H
#define BRAKEZA3D_SHADEROPENGLIMAGEOFFSET_H


#include "../../include/OpenGL/ShaderOpenGL.h"
#include "../../include/OpenGL/ShaderQuadOpenGL.h"
#include "../../include/Objects/Point2D.h"

class ShaderOpenGLImageOffset : ShaderOpenGL, public ShaderQuadOpenGL {
    GLint textureUniform;
    GLint alphaUniform;
    GLint offsetUniform;
    GLint intensityUniform;
public:
    ShaderOpenGLImageOffset();

    void render(GLuint textureID, float offsetX, float offsetY, float intensity, float alpha, GLuint framebuffer);

private:
    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLIMAGEOFFSET_H
