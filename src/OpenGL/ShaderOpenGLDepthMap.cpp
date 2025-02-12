#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/OpenGL/ShaderOpenGLDepthMap.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLDepthMap::ShaderOpenGLDepthMap()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "DepthMap.vs",
        EngineSetup::get()->SHADERS_FOLDER + "DepthMap.fs"
    )
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "depthTexture");
    intensityUniform = glGetUniformLocation(programID, "intensity");
    farPlaneUniform = glGetUniformLocation(programID, "far_plane");
}

void ShaderOpenGLDepthMap::render(GLuint textureID, GLuint framebuffer)
{
    auto render = ComponentsManager::get()->getComponentRender();

    render->changeOpenGLFramebuffer(framebuffer);

    render->changeOpenGLProgram(programID);

    glDisable(GL_DEPTH_TEST);

    loadQuadMatrixUniforms();

    auto shaderOGLDOF = render->getShaderOGLDOF();
    glUniform1f(intensityUniform, shaderOGLDOF->intensity);
    glUniform1f(farPlaneUniform, shaderOGLDOF->farPlane);

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
