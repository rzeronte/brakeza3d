#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>
#include "../../include/OpenGL/ShaderOpenGLDepthMap.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLDepthMap::ShaderOpenGLDepthMap()
:
    quadVAO(0),
    VBO(0),
    ShaderOpenGL(
            EngineSetup::get()->SHADERS_FOLDER + "DepthMap.vs",
            EngineSetup::get()->SHADERS_FOLDER + "DepthMap.fs"
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
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    modelMatrixUniform = glGetUniformLocation(programID, "model");
    projectionMatrixUniform = glGetUniformLocation(programID, "projection");
    textureUniform = glGetUniformLocation(programID, "depthTexture");

    intensityUniform = glGetUniformLocation(programID, "intensity");
    farPlaneUniform = glGetUniformLocation(programID, "far_plane");
}

void ShaderOpenGLDepthMap::render(GLuint textureID, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glDisable(GL_DEPTH_TEST);

    glUseProgram(programID);

    auto window = ComponentsManager::get()->getComponentWindow();

    glm::mat4 projection = glm::ortho(0.0f, (float) window->width, (float) window->height, 0.0f, -1.0f, 1.0f);

    glm::vec2 position = glm::vec2(0, 0);

    glm::vec2 size = glm::vec2(window->width, (float) window->height);
    float rotate = 0.0f;
    glm::vec3 color = glm::vec3(1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &projection[0][0]);

    glUniform1f(intensityUniform, ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->intensity);
    glUniform1f(farPlaneUniform, ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->farPlane);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureUniform, 0);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOpenGLDepthMap::destroy() {

}
