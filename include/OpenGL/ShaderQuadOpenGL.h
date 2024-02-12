//
// Created by edu on 27/12/23.
//

#ifndef BRAKEZA3D_SHADERQUADOPENGL_H
#define BRAKEZA3D_SHADERQUADOPENGL_H

#include <GL/glew.h>

class ShaderQuadOpenGL {
    GLuint VBO;
    GLint modelMatrixUniform;
    GLint projectionMatrixUniform;

    glm::mat4 modelMatrix;
    glm::mat4 projectionMatrix;

protected:
    GLuint quadVAO;
public:
    ShaderQuadOpenGL();

    void setupQuadUniforms(GLuint programId);

    void resetQuadMatrix();

    void loadQuadMatrixUniforms();

    void drawQuad() const;
};


#endif //BRAKEZA3D_SHADERQUADOPENGL_H
