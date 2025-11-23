
#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/OpenGL/ShaderOGLDOF.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOGLDOF::ShaderOGLDOF()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "DeepOfField.vs",
        EngineSetup::get()->SHADERS_FOLDER + "DeepOfField.fs",
        false
    ),
    resultFramebuffer(0),
    textureResult(0),
    focalRange(0),
    focalDistance(0),
    blurRadius(4),
    intensity(1.0f),
    farPlane(100.0)
{
    setupQuadUniforms(programID);
    createFramebuffer();

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

void ShaderOGLDOF::render(GLuint sceneTexture, GLuint depthTexture)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(resultFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    setTextureUniform(sceneTextureUniform, sceneTexture, 0);
    setTextureUniform(depthTextureUniform, depthTexture, 1);

    loadQuadMatrixUniforms();

    auto window = ComponentsManager::get()->getComponentWindow();

    glUniform1f(focalDistanceUniform, focalDistance);
    glUniform1f(focalRangeUniform, focalRange);
    glUniform1i(blurRadiusUniform, blurRadius);
    glUniform1f(intensityUniform, intensity);
    glUniform1f(farPlaneUniform, farPlane);
    glUniform1i(widthUniform, window->getWidth());
    glUniform1i(heightUniform, window->getHeight());

    drawQuad();
}

GLuint ShaderOGLDOF::getTextureResult() const
{
    return textureResult;
}

void ShaderOGLDOF::createFramebuffer()
{
    glGenFramebuffers(1, &resultFramebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(resultFramebuffer);
    auto window = ComponentsManager::get()->getComponentWindow();

    glGenTextures(1, &textureResult);
    glBindTexture(GL_TEXTURE_2D, textureResult);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->getWidth(), window->getHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureResult, 0);
}

void ShaderOGLDOF::destroy()
{
    glDeleteFramebuffers(1, &resultFramebuffer);
    glDeleteTextures(1, &textureResult);

    createFramebuffer();

    resetQuadMatrix();
}
