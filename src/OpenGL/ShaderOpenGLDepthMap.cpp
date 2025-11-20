#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/OpenGL/ShaderOpenGLDepthMap.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLDepthMap::ShaderOpenGLDepthMap()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "DepthMap.vs",
        EngineSetup::get()->SHADERS_FOLDER + "DepthMap.fs",
        false
    ),
    intensity(10.0f),
    farPlane(EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE),
    nearPlane(0.1f)
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "depthTexture");
    intensityUniform = glGetUniformLocation(programID, "intensity");
    nearUniform = glGetUniformLocation(programID, "near");
    farUniform = glGetUniformLocation(programID, "far");
}

void ShaderOpenGLDepthMap::render(GLuint textureID, GLuint framebuffer)
{
    auto render = ComponentsManager::get()->getComponentRender();
    render->changeOpenGLFramebuffer(framebuffer);
    render->changeOpenGLProgram(programID);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    loadQuadMatrixUniforms();

    glUniform1f(nearUniform, nearPlane);
    glUniform1f(farUniform, farPlane);
    glUniform1f(intensityUniform, intensity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureUniform, 0);

    drawQuad();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    render->changeOpenGLFramebuffer(0);
}

void ShaderOpenGLDepthMap::destroy()
{
    resetQuadMatrix();
}
