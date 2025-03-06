#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/OpenGL/ShaderOpenGLDepthMap.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLDepthMap::ShaderOpenGLDepthMap()
:
    intensity(1.0f),
    nearPlane(0.1f),
    farPlane(100.0f),
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "DepthMap.vs",
        EngineSetup::get()->SHADERS_FOLDER + "DepthMap.fs",
        false
    )
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "depthTexture");
    intensityUniform = glGetUniformLocation(programID, "intensity");
    farPlaneUniform = glGetUniformLocation(programID, "farPlane");
    nearPlaneUniform = glGetUniformLocation(programID, "nearPlane");
}

void ShaderOpenGLDepthMap::render(GLuint textureID, GLuint framebuffer)
{
    auto render = ComponentsManager::get()->getComponentRender();

    render->changeOpenGLFramebuffer(framebuffer);

    render->changeOpenGLProgram(programID);

    glDisable(GL_DEPTH_TEST);

    loadQuadMatrixUniforms();

    glUniform1f(intensityUniform, intensity);
    glUniform1f(farPlaneUniform, farPlane);
    glUniform1f(nearPlaneUniform, nearPlane);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureUniform, 0);

    drawQuad();

    glEnable(GL_DEPTH_TEST);

    render->changeOpenGLFramebuffer(0);
}

void ShaderOpenGLDepthMap::destroy()
{
    resetQuadMatrix();
}
