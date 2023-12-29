
#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>
#include "../../include/OpenGL/ShaderOpenGLDOF.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLDOF::ShaderOpenGLDOF()
:
    VBO(0),
    quadVAO(0),
    focalRange(0),
    focalDistance(0),
    blurRadius(10),
    farPlane(100.0),
    intensity(1.0f),
    resultFramebuffer(0),
    textureResult(0),
    ShaderOpenGL(
            EngineSetup::get()->SHADERS_FOLDER + "DeepOfField.vs",
            EngineSetup::get()->SHADERS_FOLDER + "DeepOfField.fs"
    )
{
    createFramebuffer();

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

    modelMatrixUniform = glGetUniformLocation(programID, "model");
    projectionMatrixUniform = glGetUniformLocation(programID, "projection");

    focalDistanceUniform = glGetUniformLocation(programID, "focalDistance");
    focalRangeUniform = glGetUniformLocation(programID, "focalRange");
    blurRadiusUniform = glGetUniformLocation(programID, "blurRadius");
    intensityUniform = glGetUniformLocation(programID, "intensity");
    farPlaneUniform = glGetUniformLocation(programID, "far_plane");

    depthTextureUniform = glGetUniformLocation(programID, "depthTexture");
    sceneTextureUniform = glGetUniformLocation(programID, "sceneTexture");

    widthUniform = glGetUniformLocation(programID, "screenWidth");
    heightUniform = glGetUniformLocation(programID, "screenHeight");
}

void ShaderOpenGLDOF::render(GLuint sceneTexture, GLuint depthTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, resultFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glUniform1i(sceneTextureUniform, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(depthTextureUniform, 1);

    const int x = 0;
    const int y = 0;
    auto window = ComponentsManager::get()->getComponentWindow();
    const int w = window->getWidth();
    const int h = window->getHeight();

    glm::mat4 projection = glm::ortho(0.0f, (float) w, (float) h, 0.0f, -1.0f, 1.0f);

    glm::vec2 position = glm::vec2(x, y);
    glm::vec2 size = glm::vec2(w, h);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &projection[0][0]);

    glUniform1f(focalDistanceUniform, focalDistance);
    glUniform1f(focalRangeUniform, focalRange);
    glUniform1i(blurRadiusUniform, blurRadius);
    glUniform1f(intensityUniform, intensity);
    glUniform1f(farPlaneUniform, farPlane);
    glUniform1i(widthUniform, w);
    glUniform1i(heightUniform, h);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

GLuint ShaderOpenGLDOF::getTextureResult() const {
    return textureResult;
}

void ShaderOpenGLDOF::createFramebuffer()
{
    glGenFramebuffers(1, &resultFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, resultFramebuffer);

    int w, h;
    SDL_GetWindowSize(ComponentsManager::get()->getComponentWindow()->getWindow(), &w, &h);

    glGenTextures(1, &textureResult);
    glBindTexture(GL_TEXTURE_2D, textureResult);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureResult, 0);
}

void ShaderOpenGLDOF::destroy()
{
    glDeleteFramebuffers(1, &resultFramebuffer);
    glDeleteTextures(1, &textureResult);

    createFramebuffer();
}
