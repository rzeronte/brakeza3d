#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/OpenGL/ShaderOGLDepthMap.h"
#include "../../include/Config.h"
#include "../../include/Components/ComponentsManager.h"

ShaderOGLDepthMap::ShaderOGLDepthMap()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "DepthMap.vs",
        Config::get()->SHADERS_FOLDER + "DepthMap.fs",
        false
    ),
    intensity(10.0f),
    farPlane(Config::get()->FRUSTUM_FARPLANE_DISTANCE),
    nearPlane(0.1f)
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "depthTexture");
    intensityUniform = glGetUniformLocation(programID, "intensity");
    nearUniform = glGetUniformLocation(programID, "near");
    farUniform = glGetUniformLocation(programID, "far");
}

void ShaderOGLDepthMap::render(GLuint textureID, GLuint fbo)
{
    auto render = ComponentsManager::get()->getComponentRender();
    render->changeOpenGLFramebuffer(fbo);
    render->changeOpenGLProgram(programID);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    loadQuadMatrixUniforms();

    glUniform1f(nearUniform, nearPlane);
    glUniform1f(farUniform, farPlane);
    glUniform1f(intensityUniform, intensity);

    setTextureUniform(textureUniform, textureID, 0);

    drawQuad();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    render->changeOpenGLFramebuffer(0);
}

void ShaderOGLDepthMap::destroy()
{
    resetQuadMatrix();
}
