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
    intensity(1.0f),
    farPlane(100.0f),
    nearPlane(0.1f)
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "depthTexture");
}

void ShaderOpenGLDepthMap::render(GLuint textureID, GLuint framebuffer)
{
    auto render = ComponentsManager::get()->getComponentRender();

    render->changeOpenGLFramebuffer(framebuffer);

    render->changeOpenGLProgram(programID);

    glDisable(GL_DEPTH_TEST);

    loadQuadMatrixUniforms();

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
