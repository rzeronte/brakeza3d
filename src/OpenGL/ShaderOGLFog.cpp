//
// Created by edu on 26/12/23.
//

#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/OpenGL/ShaderOGLFog.h"
#include "../../include/Config.h"
#include "../../include/Components/ComponentsManager.h"

ShaderOGLFog::ShaderOGLFog()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "FOG.vs",
        Config::get()->SHADERS_FOLDER + "FOG.fs",
        false
    ),
    resultFramebuffer(0),
    textureResult(0),
    fogMaxDist(50.0f),
    fogMinDist(0.1f),
    intensity(1.0f)
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

void ShaderOGLFog::render(GLuint sceneTexture, GLuint depthTexture)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(resultFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    glUniform1f(fogMaxDistUniform, fogMaxDist);
    glUniform1f(fogMinDistUniform, fogMinDist);
    glUniform1f(intensityUniform, intensity);
    glUniform1f(farPlaneUniform, ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->farPlane);
    glUniform3fv(fogColourUniform, 1, &fogColor.toGLM()[0]);

    setTextureUniform(sceneTextureUniform, sceneTexture, 0);
    setTextureUniform(depthTextureUniform, depthTexture, 1);

    drawQuad();
}

GLuint ShaderOGLFog::getTextureResult() const
{
    return textureResult;
}

void ShaderOGLFog::destroy()
{
    resetQuadMatrix();

    glDeleteFramebuffers(1, &resultFramebuffer);
    glDeleteTextures(1, &textureResult);
    createFramebuffer();
}

void ShaderOGLFog::createFramebuffer()
{
    glGenFramebuffers(1, &resultFramebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(resultFramebuffer);

    auto window = ComponentsManager::get()->getComponentWindow();
    int w = window->getWidth();
    int h = window->getHeight();

    glGenTextures(1, &textureResult);
    glBindTexture(GL_TEXTURE_2D, textureResult);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureResult, 0);
}