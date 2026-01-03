//
// Created by edu on 27/12/23.
//

#ifndef BRAKEZA3D_SHADERQUADOPENGL_H
#define BRAKEZA3D_SHADERQUADOPENGL_H

#include <GL/glew.h>

enum ShaderBaseQuadSize {
    WINDOW_SIZE,
    RENDER_SIZE
};

class ShaderBaseOpenGLQuad
{
    GLuint VBO  = 0;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    ShaderBaseQuadSize quadSize = WINDOW_SIZE;
protected:
    GLuint modelMatrixUniform = 0;
    GLuint projectionMatrixUniform = 0;
    GLuint quadVAO = 0;
public:

    ShaderBaseOpenGLQuad() = default;
    void CreateQuadVBO();

    void SetupQuadUniforms(GLuint programId);

    int getWidthByQuadSize() const;

    int getHeightByQuadSize() const;

    void ResetQuadMatrix();
    void LoadQuadMatrixUniforms();
    void DrawQuad() const;

    [[nodiscard]] ShaderBaseQuadSize getQuadSize() const { return quadSize; }

    void setQuadSize(ShaderBaseQuadSize value);
};


#endif //BRAKEZA3D_SHADERQUADOPENGL_H
