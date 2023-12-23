
#ifndef BRAKEZA3D_SHADEROPENGLOUTLINER_H
#define BRAKEZA3D_SHADEROPENGLOUTLINER_H


#include "ShaderOpenGL.h"
#include "../Objects/Object3D.h"

class ShaderOpenGLOutliner : public ShaderOpenGL {
    GLuint VertexArrayID;

    GLuint quadVAO;
    GLuint VBO;

    GLint modelMatrixUniform;
    GLint projectionMatrixUniform;

    GLint textureUniform;

public:
    ShaderOpenGLOutliner();

    void render(Object3D *o, GLint textureID);

};


#endif //BRAKEZA3D_SHADEROPENGLOUTLINER_H
