#include <ext/matrix_float4x4.hpp>
#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>
#include "ShaderOpenGLImage.h"
#include "../EngineSetup.h"
#include "../ComponentsManager.h"

ShaderOpenGLImage::ShaderOpenGLImage()
:
    quadVAO(0),
    VBO(0),
    ShaderOpenGL("../shaders/Image.vertexshader","../shaders/Image.fragmentshader")
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
    glBindVertexArray(0);

    modelMatrixUniform = glGetUniformLocation(programID, "model");
    projectionMatrixUniform = glGetUniformLocation(programID, "projection");
    spriteColorUniform = glGetUniformLocation(programID, "spriteColor");
    textureUniform = glGetUniformLocation(programID, "image");
    alphaUniform = glGetUniformLocation(programID, "alpha");
    inverseUniform = glGetUniformLocation(programID, "inverse");
}

void ShaderOpenGLImage::renderTexture(GLuint TextureID, int x, int y, int w, int h, float alpha, bool inverse, GLuint framebuffer, float z) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glDisable(GL_DEPTH);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(programID);

    glm::mat4 projection = glm::ortho(0.0f, (float) EngineSetup::get()->screenWidth, (float) EngineSetup::get()->screenHeight, 0.0f, -1.0f, 1.0f);

    glm::vec2 position = glm::vec2(x, y);

    glm::vec2 size = glm::vec2(w, h);
    float rotate = 0.0f;
    glm::vec3 color = glm::vec3(1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, z));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &projection[0][0]);
    glUniform3fv(spriteColorUniform, 1, &color[0]);
    glUniform1fv(alphaUniform, 1, &alpha);
    glUniform1i(inverseUniform, inverse);
    setInt("depth", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    glUniform1i(textureUniform, 0);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
}
