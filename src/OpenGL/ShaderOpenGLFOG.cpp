//
// Created by edu on 26/12/23.
//

#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>
#include "../../include/OpenGL/ShaderOpenGLFOG.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLFOG::ShaderOpenGLFOG()
:
    resultFramebuffer(0),
    textureResult(0),
    fogMinDist(0.1f),
    fogMaxDist(150.0f),
    intensity(1.0f),
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "FOG.vs",
        EngineSetup::get()->SHADERS_FOLDER + "FOG.fs"
    )
{
    createFramebuffer();

    setupQuadUniforms(programID);

    fogMaxDistUniform = glGetUniformLocation(programID, "fogMaxDist");
    fogMinDistUniform = glGetUniformLocation(programID, "fogMinDist");
    fogColourUniform = glGetUniformLocation(programID, "fogColor");

    depthTextureUniform = glGetUniformLocation(programID, "depthTexture");
    sceneTextureUniform = glGetUniformLocation(programID, "sceneTexture");

    intensityUniform = glGetUniformLocation(programID, "intensity");
    farPlaneUniform = glGetUniformLocation(programID, "farPlane");
}

void ShaderOpenGLFOG::render(GLuint sceneTexture, GLuint depthTexture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, resultFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);

    loadQuadMatrixUniforms();

    glUniform1f(fogMaxDistUniform, fogMaxDist);
    glUniform1f(fogMinDistUniform, fogMinDist);
    glUniform1f(intensityUniform, intensity);
    glUniform1f(farPlaneUniform, ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->farPlane);
    glUniform3fv(fogColourUniform, 1, &fogColor.toGLM()[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glUniform1i(sceneTextureUniform, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(depthTextureUniform, 1);

    drawQuad();
}

GLuint ShaderOpenGLFOG::getTextureResult() const {
    return textureResult;
}

void ShaderOpenGLFOG::destroy()
{
    resetQuadMatrix();

    glDeleteFramebuffers(1, &resultFramebuffer);
    glDeleteTextures(1, &textureResult);
    createFramebuffer();
}

void ShaderOpenGLFOG::createFramebuffer()
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