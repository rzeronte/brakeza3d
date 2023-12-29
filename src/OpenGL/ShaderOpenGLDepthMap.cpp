#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>
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
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);

    glDisable(GL_DEPTH_TEST);

    loadQuadMatrixUniforms();

    glUniform1f(intensityUniform, ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->intensity);
    glUniform1f(farPlaneUniform, ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->farPlane);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureUniform, 0);

    drawQuad();

    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOpenGLDepthMap::destroy()
{
    resetQuadMatrix();
}
