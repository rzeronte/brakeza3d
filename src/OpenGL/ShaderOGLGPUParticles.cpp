#include <cstddef>
#include "../../include/OpenGL/ShaderOGLGPUParticles.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/ParticlesContext.h"

ShaderOGLGPUParticles::ShaderOGLGPUParticles()
    : ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "ParticleGPU.vs",
        Config::get()->SHADERS_FOLDER + "ParticleGPU.fs",
        false
    )
{
}

void ShaderOGLGPUParticles::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    glGenVertexArrays(1, &VAO);
    LoadUniforms();
}

void ShaderOGLGPUParticles::LoadUniforms()
{
    uCameraRight    = glGetUniformLocation(programID, "CameraRight_worldspace");
    uCameraUp       = glGetUniformLocation(programID, "CameraUp_worldspace");
    uVP             = glGetUniformLocation(programID, "VP");
    uTextureSampler = glGetUniformLocation(programID, "myTextureSampler");
    uColorFrom      = glGetUniformLocation(programID, "colorFrom");
    uColorTo        = glGetUniformLocation(programID, "colorTo");
}

void ShaderOGLGPUParticles::render(
    GLuint particlesStateBuffer,
    GLuint billboardVBO,
    GLuint textureID,
    const Color& colorFrom,
    const Color& colorTo,
    int numParticles,
    int stride
)
{
    if (programID == 0) {
        LOG_ERROR("[ShaderOGLGPUParticles] programID=0, GPU render shader not compiled");
        return;
    }

    Components::get()->Render()->ChangeOpenGLFramebuffer(Components::get()->Window()->getForegroundFramebuffer());

    glUseProgram(programID);
    Components::get()->Render()->setLastProgramUsed(programID);

    glBindVertexArray(VAO);

    glm::mat4 proj = Components::get()->Camera()->getGLMMat4ProjectionMatrix();
    glm::mat4 view = Components::get()->Camera()->getGLMMat4ViewMatrix();
    glm::mat4 vp   = proj * view;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    setTextureUniform(uTextureSampler, textureID, 0);

    glUniform3f(uCameraRight, view[0][0], view[1][0], view[2][0]);
    glUniform3f(uCameraUp,    view[0][1], view[1][1], view[2][1]);
    glUniformMatrix4fv(uVP, 1, GL_FALSE, &vp[0][0]);

    glUniform4f(uColorFrom, colorFrom.r, colorFrom.g, colorFrom.b, colorFrom.a);
    glUniform4f(uColorTo,   colorTo.r,   colorTo.g,   colorTo.b,   colorTo.a);

    const int OCPARTICLE_STRIDE = stride;

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribDivisor(0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particlesStateBuffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, OCPARTICLE_STRIDE, (void*)0);
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, OCPARTICLE_STRIDE, (void*)offsetof(OCParticle, timeToLive));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, OCPARTICLE_STRIDE, (void*)offsetof(OCParticle, timeLiving));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, OCPARTICLE_STRIDE, (void*)offsetof(OCParticle, active));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, OCPARTICLE_STRIDE, (void*)offsetof(OCParticle, force));
    glVertexAttribDivisor(5, 1);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numParticles);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLGPUParticles::Destroy()
{
    if (VAO != 0) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
}
