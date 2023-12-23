
#include <trigonometric.hpp>
#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>
#include "ShaderOpenGLOutliner.h"

ShaderOpenGLOutliner::ShaderOpenGLOutliner()
:
    ShaderOpenGL("../shaders/Outliner.vertexshader","../shaders/Outliner.fragmentshader")
{
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    float vertices[] = {
            // pos      // tex
            0.0f, 1.0f, 0.0f, 0.0f, // Cambia la coordenada v de la textura aquí
            1.0f, 0.0f, 1.0f, 1.0f, // Y aquí
            0.0f, 0.0f, 0.0f, 1.0f, // Y aquí

            0.0f, 1.0f, 0.0f, 0.0f, // Y aquí
            1.0f, 1.0f, 1.0f, 0.0f, // Y aquí
            1.0f, 0.0f, 1.0f, 1.0f  // Y aquí
    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    modelMatrixUniform = glGetUniformLocation(programID, "model");
    projectionMatrixUniform = glGetUniformLocation(programID, "projection");

    textureUniform = glGetUniformLocation(programID, "image");
}

void ShaderOpenGLOutliner::render(
    Object3D *o,
    GLint textureID
)
{
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(programID);

    glm::mat4 projection = glm::ortho(0.0f, (float) EngineSetup::get()->screenWidth, (float) EngineSetup::get()->screenHeight, 0.0f, -1.0f, 1.0f);
    glm::vec2 position = glm::vec2(0, 0);

    glm::vec2 size = glm::vec2(EngineSetup::get()->screenWidth, EngineSetup::get()->screenHeight);
    float rotate = 0.0f;
    glm::vec3 color = glm::vec3(1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &projection[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureUniform, 0);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_BLEND);

    glBindVertexArray(0);
}
