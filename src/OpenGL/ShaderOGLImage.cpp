#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/OpenGL/ShaderOGLImage.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOGLImage::ShaderOGLImage()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Image.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Image.fs",
        false
    )
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
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    modelMatrixUniform = glGetUniformLocation(programID, "model");
    projectionMatrixUniform = glGetUniformLocation(programID, "projection");
    textureUniform = glGetUniformLocation(programID, "image");
    alphaUniform = glGetUniformLocation(programID, "alpha");
    inverseUniform = glGetUniformLocation(programID, "inverse");
}

void ShaderOGLImage::renderTexture(GLuint textureId, int x, int y, int w, int h, float alpha, bool inverse, GLuint fbo) const
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(fbo);

    glDisable(GL_DEPTH_TEST);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    auto window = ComponentsManager::get()->getComponentWindow();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window->getWidth()), static_cast<float>(window->getHeight()), 0.0f, -1.0f, 1.0f);

    glm::vec2 position = glm::vec2(x, y);
    glm::vec2 size = glm::vec2(w, h);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &projection[0][0]);
    glUniform1fv(alphaUniform, 1, &alpha);
    glUniform1i(inverseUniform, inverse);

    setTextureUniform(textureUniform, textureId, 0);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(0);
}

void ShaderOGLImage::destroy() {

}
