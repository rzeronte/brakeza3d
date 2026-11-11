#include <cstddef>
#include "../../include/OpenGL/ShaderOGLGPUParticles.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/ParticlesContext.h"
#include "../../include/Render/Profiler.h"

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
    uSizeStart      = glGetUniformLocation(programID, "sizeStart");
    uSizeEnd        = glGetUniformLocation(programID, "sizeEnd");
    uOrientation    = glGetUniformLocation(programID, "orientation");
    uFlatRight      = glGetUniformLocation(programID, "flatRight");
    uFlatUp         = glGetUniformLocation(programID, "flatUp");
    uCameraPos      = glGetUniformLocation(programID, "cameraPos");
    uFlutterAngle   = glGetUniformLocation(programID, "flutterAngle");
    uFlutterSpeed   = glGetUniformLocation(programID, "flutterSpeed");
}

void ShaderOGLGPUParticles::render(
    GLuint particlesStateBuffer,
    GLuint billboardVBO,
    GLuint textureID,
    const Color& colorFrom,
    const Color& colorTo,
    float sizeStart,
    float sizeEnd,
    int numParticles,
    int stride,
    bool depthTest,
    int orientation,
    const glm::vec3 &flatRight,
    const glm::vec3 &flatUp,
    float flutterAngle,
    float flutterSpeed
)
{
    if (programID == 0) {
        LOG_ERROR("[ShaderOGLGPUParticles] programID=0, GPU render shader not compiled");
        return;
    }

    // depthTest: el sceneFBO comparte el depth del G-Buffer, así la geometría opaca oculta las partículas.
    // El foregroundFBO no tiene profundidad, por eso allí siempre quedan por encima de todo.
    auto* window = Components::get()->Window();
    Components::get()->Render()->ChangeOpenGLFramebuffer(
        depthTest ? window->getSceneFramebuffer() : window->getForegroundFramebuffer()
    );

    glUseProgram(programID);
    Components::get()->Render()->setLastProgramUsed(programID);
    Profiler::get()->incrementProgramChanges();

    glBindVertexArray(VAO);

    glm::mat4 proj = Components::get()->Camera()->getGLMMat4ProjectionMatrix();
    glm::mat4 view = Components::get()->Camera()->getGLMMat4ViewMatrix();
    glm::mat4 vp   = proj * view;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (depthTest) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);      // prueba contra la escena pero no escribe: las partículas no se tapan entre sí
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    setTextureUniform(uTextureSampler, textureID, 0);

    glUniform3f(uCameraRight, view[0][0], view[1][0], view[2][0]);
    glUniform3f(uCameraUp,    view[0][1], view[1][1], view[2][1]);
    glUniformMatrix4fv(uVP, 1, GL_FALSE, &vp[0][0]);

    glUniform4f(uColorFrom, colorFrom.r, colorFrom.g, colorFrom.b, colorFrom.a);
    glUniform4f(uColorTo,   colorTo.r,   colorTo.g,   colorTo.b,   colorTo.a);
    glUniform1f(uSizeStart, sizeStart);
    glUniform1f(uSizeEnd,   sizeEnd);
    glUniform1i(uOrientation, orientation);
    glUniform3f(uFlatRight, flatRight.x, flatRight.y, flatRight.z);
    glUniform3f(uFlatUp,    flatUp.x,    flatUp.y,    flatUp.z);
    const glm::vec3 cameraPos = -glm::transpose(glm::mat3(view)) * glm::vec3(view[3]);
    glUniform3f(uCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform1f(uFlutterAngle, flutterAngle);
    glUniform1f(uFlutterSpeed, flutterSpeed);

    // Quad plano (no billboard): se ve por las dos caras; con culling desaparecería. Solo en ese modo se toca el estado
    const bool restoreCulling = orientation != 0 && glIsEnabled(GL_CULL_FACE) == GL_TRUE;
    if (orientation != 0) glDisable(GL_CULL_FACE);

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

    // loc 6: ángulo (OCParticle.rotation.x, radianes)
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, OCPARTICLE_STRIDE, (void*)offsetof(OCParticle, rotation));
    glVertexAttribDivisor(6, 1);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numParticles);
    Profiler::get()->incrementDrawCall(GL_TRIANGLE_STRIP, 4, numParticles, "GPUParticles");

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (restoreCulling) glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLGPUParticles::Destroy()
{
    if (VAO != 0) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
}
