//
// Created by edu on 10/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLLINE_H
#define BRAKEZA3D_SHADEROPENGLLINE_H


#include "ShaderOpenGL.h"
#include "../Objects/Point2D.h"
#include "../Misc/Color.h"

class ShaderOpenGLLine : public ShaderOpenGL {
    GLuint quadVAO;
    GLuint VBO;

    GLint modelMatrixUniform;
    GLint projectionMatrixUniform;
public:
    ShaderOpenGLLine();

    void render(Point2D a, Point2D b, Color c, float weight, GLuint framebuffer);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLLINE_H
