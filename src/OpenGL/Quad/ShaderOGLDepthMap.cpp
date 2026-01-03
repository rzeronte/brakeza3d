#include <glm/ext/matrix_clip_space.hpp>
#include "../../../include/OpenGL/Quad/ShaderOGLDepthMap.h"
#include "../../../include/Config.h"
#include "../../../include/Components/Components.h"

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
}

void ShaderOGLDepthMap::LoadUniforms()
{
    textureUniform = glGetUniformLocation(programID, "depthTexture");
    intensityUniform = glGetUniformLocation(programID, "intensity");
    nearUniform = glGetUniformLocation(programID, "near");
    farUniform = glGetUniformLocation(programID, "far");
}

void ShaderOGLDepthMap::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateQuadVBO();
    SetupQuadUniforms(programID);
}

void ShaderOGLDepthMap::Render(GLuint textureID, GLuint fbo)
{
    auto render = Components::get()->Render();
    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    LoadQuadMatrixUniforms();

    glUniform1f(nearUniform, nearPlane);
    glUniform1f(farUniform, farPlane);
    glUniform1f(intensityUniform, intensity);

    setTextureUniform(textureUniform, textureID, 0);

    DrawQuad();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    render->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLDepthMap::Destroy()
{
    ResetQuadMatrix();
}
