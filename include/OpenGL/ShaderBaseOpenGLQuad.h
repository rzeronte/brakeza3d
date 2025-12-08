//
// Created by edu on 27/12/23.
//

#ifndef BRAKEZA3D_SHADERQUADOPENGL_H
#define BRAKEZA3D_SHADERQUADOPENGL_H

#include <GL/glew.h>

class ShaderBaseOpenGLQuad {
    GLuint VBO;

    glm::mat4 modelMatrix;
    glm::mat4 projectionMatrix;

protected:
    GLuint modelMatrixUniform;
    GLuint projectionMatrixUniform;
    GLuint quadVAO;
public:
    ShaderBaseOpenGLQuad();

    void setupQuadUniforms(GLuint programId);
    void resetQuadMatrix();
    void loadQuadMatrixUniforms();
    void drawQuad() const;
};


#endif //BRAKEZA3D_SHADERQUADOPENGL_H
