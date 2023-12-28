//
// Created by edu on 28/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLLINELASER_H
#define BRAKEZA3D_SHADEROPENGLLINELASER_H


#include "../../include/OpenGL/ShaderOpenGL.h"
#include "../../include/Objects/Point2D.h"
#include "../../include/Misc/Color.h"
#include "../../include/Misc/Image.h"

class ShaderOpenGLLineLaser : public ShaderOpenGL  {
    GLuint quadVAO;
    GLuint VBO;

    GLint modelMatrixUniform;
    GLint projectionMatrixUniform;
    GLint textureUniform;
    Image *image;
public:
    ShaderOpenGLLineLaser();

    void render(Point2D a, Point2D b, Color c, float weight, GLuint framebuffer);

    void destroy() override;

};


#endif //BRAKEZA3D_SHADEROPENGLLINELASER_H
