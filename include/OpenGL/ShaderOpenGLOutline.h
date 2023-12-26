
#ifndef BRAKEZA3D_SHADEROPENGLOUTLINE_H
#define BRAKEZA3D_SHADEROPENGLOUTLINE_H


#include "ShaderOpenGL.h"
#include "../Objects/Object3D.h"

class ShaderOpenGLOutline : public ShaderOpenGL {

    Color lineColor;

    GLuint quadVAO;
    GLuint VBO;

    GLint modelMatrixUniform;
    GLint projectionMatrixUniform;

    GLint textureUniform;
    GLint lineColorUniform;

public:
    ShaderOpenGLOutline();

    void render(GLint textureID);

};


#endif //BRAKEZA3D_SHADEROPENGLOUTLINE_H
