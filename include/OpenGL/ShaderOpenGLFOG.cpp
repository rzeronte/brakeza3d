//
// Created by edu on 26/12/23.
//

#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>
#include "ShaderOpenGLFOG.h"
#include "../EngineSetup.h"
#include "../ComponentsManager.h"

ShaderOpenGLFOG::ShaderOpenGLFOG()
:
    fogMinDist(0.1f),
    fogMaxDist(8.0f),
    ShaderOpenGL("../shaders/FOG.vertexshader", "../shaders/FOG.fragmentshader")
{
    glGenFramebuffers(1, &resultFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, resultFramebuffer);

    int w = EngineSetup::get()->screenWidth, h = EngineSetup::get()->screenHeight;

    glGenTextures(1, &textureResult);
    glBindTexture(GL_TEXTURE_2D, textureResult);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureResult, 0);

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

    fogMaxDistUniform = glGetUniformLocation(programID, "fogMaxDist");
    fogMinDistUniform = glGetUniformLocation(programID, "fogMinDist");;
    fogColourUniform = glGetUniformLocation(programID, "fogColor");;

    depthTextureUniform = glGetUniformLocation(programID, "depthTexture");
    sceneTextureUniform = glGetUniformLocation(programID, "sceneTexture");

    intensityUniform = glGetUniformLocation(programID, "intensity");
    farPlaneUniform = glGetUniformLocation(programID, "farPlane");;
}

void ShaderOpenGLFOG::render(GLuint sceneTexture, GLuint depthTexture)
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
    const int w = EngineSetup::get()->screenWidth;
    const int h = EngineSetup::get()->screenHeight;

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

    glUniform1f(fogMaxDistUniform, fogMaxDist);
    glUniform1f(fogMinDistUniform, fogMinDist);
    glUniform1f(intensityUniform, intensity);
    glUniform1f(farPlaneUniform, ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->farPlane);
    glUniform3fv(fogColourUniform, 1, &fogColor.toGLM()[0]);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

GLuint ShaderOpenGLFOG::getTextureResult() const {
    return textureResult;
}
