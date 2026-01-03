
#include <glm/ext/matrix_clip_space.hpp>
#include "../../../include/OpenGL/Quad/ShaderOGLDOF.h"
#include "../../../include/Config.h"
#include "../../../include/Components/Components.h"

ShaderOGLDOF::ShaderOGLDOF()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "DeepOfField.vs",
        Config::get()->SHADERS_FOLDER + "DeepOfField.fs",
        false
    ),
    blurRadius(4),
    intensity(1.0f),
    farPlane(100.0)
{
}

void ShaderOGLDOF::LoadUniforms()
{
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

void ShaderOGLDOF::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateQuadVBO();
    SetupQuadUniforms(programID);
    CreateFramebuffer();
}

void ShaderOGLDOF::render(GLuint sceneTexture, GLuint depthTexture)
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(resultFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Components::get()->Render()->ChangeOpenGLProgram(programID);

    setTextureUniform(sceneTextureUniform, sceneTexture, 0);
    setTextureUniform(depthTextureUniform, depthTexture, 1);

    LoadQuadMatrixUniforms();

    auto window = Components::get()->Window();

    glUniform1f(focalDistanceUniform, focalDistance);
    glUniform1f(focalRangeUniform, focalRange);
    glUniform1i(blurRadiusUniform, blurRadius);
    glUniform1f(intensityUniform, intensity);
    glUniform1f(farPlaneUniform, farPlane);
    glUniform1i(widthUniform, window->getWidth());
    glUniform1i(heightUniform, window->getHeight());

    DrawQuad();
}

GLuint ShaderOGLDOF::getTextureResult() const
{
    return textureResult;
}

void ShaderOGLDOF::CreateFramebuffer()
{
    glGenFramebuffers(1, &resultFramebuffer);
    Components::get()->Render()->ChangeOpenGLFramebuffer(resultFramebuffer);
    auto window = Components::get()->Window();

    glGenTextures(1, &textureResult);
    glBindTexture(GL_TEXTURE_2D, textureResult);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->getWidthRender(), window->getHeightRender(), 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureResult, 0);
}

void ShaderOGLDOF::Destroy()
{
    glDeleteFramebuffers(1, &resultFramebuffer);
    glDeleteTextures(1, &textureResult);

    CreateFramebuffer();

    ResetQuadMatrix();
}
