//
// Created by edu on 27/12/23.
//

#ifndef BRAKEZA3D_SHADERQUADOPENGL_H
#define BRAKEZA3D_SHADERQUADOPENGL_H

#include <GL/glew.h>

class ShaderBaseOpenGLQuad
{
    GLuint VBO  = 0;

    glm::mat4 modelMatrix;
    glm::mat4 projectionMatrix;

protected:
    GLuint modelMatrixUniform;
    GLuint projectionMatrixUniform;
    GLuint quadVAO = 0;
public:

    ShaderBaseOpenGLQuad();
    void CreateQuadVBO();

    void SetupQuadUniforms(GLuint programId);
    void ResetQuadMatrix();
    void LoadQuadMatrixUniforms();
    void DrawQuad() const;
};


#endif //BRAKEZA3D_SHADERQUADOPENGL_H
