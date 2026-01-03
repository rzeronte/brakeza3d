#define GL_GLEXT_PROTOTYPES

//
// Created by edu on 27/12/23.
//

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../../include/OpenGL/Base/ShaderBaseOpenGLQuad.h"
#include "../../../include/Components/Components.h"


void ShaderBaseOpenGLQuad::SetupQuadUniforms(GLuint programID)
{
    modelMatrixUniform = glGetUniformLocation(programID, "model");
    projectionMatrixUniform = glGetUniformLocation(programID, "projection");
    ResetQuadMatrix();
}

int ShaderBaseOpenGLQuad::getWidthByQuadSize() const
{
    auto window = Components::get()->Window();
    int x = window->getWidth();
    if (getQuadSize() == RENDER_SIZE) {
        x = window->getWidthRender();
    }

    return x;

}

int ShaderBaseOpenGLQuad::getHeightByQuadSize() const
{
    auto window = Components::get()->Window();
    int y = window->getHeight();
    if (getQuadSize() == RENDER_SIZE) {
        y = window->getHeightRender();
    }

    return y;
}

void ShaderBaseOpenGLQuad::CreateQuadVBO()
{
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ShaderBaseOpenGLQuad::ResetQuadMatrix()
{
    int w = this->getWidthByQuadSize();
    int h = this->getHeightByQuadSize();

    glm::mat4 projection = glm::ortho(0.0f, (float) w, (float) h, 0.0f, -1.0f, 1.0f);

    glm::vec2 position = glm::vec2(0, 0);
    glm::vec2 size = glm::vec2(w, h);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    modelMatrix = model;
    projectionMatrix = projection;
}

void ShaderBaseOpenGLQuad::LoadQuadMatrixUniforms()
{
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void ShaderBaseOpenGLQuad::DrawQuad() const
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void ShaderBaseOpenGLQuad::setQuadSize(ShaderBaseQuadSize value)
{
    quadSize = value;
}
