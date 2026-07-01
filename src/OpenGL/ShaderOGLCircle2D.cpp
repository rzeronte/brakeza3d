#include <SDL2/SDL.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/OpenGL/ShaderOGLCircle2D.h"
#include "../../include/Config.h"
#include "../../include/Components/Components.h"

ShaderOGLCircle2D::ShaderOGLCircle2D()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Image.vs",
        Config::get()->SHADERS_FOLDER + "Circle2D.fs",
        false
    )
{
}

void ShaderOGLCircle2D::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    CreateQuadVBO();
    LoadUniforms();
}

void ShaderOGLCircle2D::LoadUniforms()
{
    modelMatrixUniform      = glGetUniformLocation(programID, "model");
    projectionMatrixUniform = glGetUniformLocation(programID, "projection");
    waveColorUniform        = glGetUniformLocation(programID, "waveColor");
    timeUniform             = glGetUniformLocation(programID, "time");
    numWavesUniform         = glGetUniformLocation(programID, "numWaves");
    speedUniform            = glGetUniformLocation(programID, "speed");
    thicknessUniform        = glGetUniformLocation(programID, "thickness");
}

void ShaderOGLCircle2D::CreateQuadVBO()
{
    float vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ShaderOGLCircle2D::renderCircle2D(
    int x, int y, int w, int h,
    int worldW, int worldH,
    const Color &c,
    float numWaves, float speed, float thickness,
    bool additive,
    GLuint fbo
) const
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    glViewport(0, 0, worldW, worldH);
    glDisable(GL_DEPTH_TEST);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    glm::mat4 projection = glm::ortho(0.0f, (float)worldW, (float)worldH, 0.0f, -1.0f, 1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3((float)x, (float)y, 0.0f));
    model = glm::scale(model, glm::vec3((float)w, (float)h, 1.0f));

    const float timeSec = (float)SDL_GetTicks() / 1000.0f;

    glUniformMatrix4fv(modelMatrixUniform,      1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &projection[0][0]);
    glUniform4f(waveColorUniform, c.r, c.g, c.b, c.a);
    glUniform1f(timeUniform,      timeSec);
    glUniform1f(numWavesUniform,  numWaves);
    glUniform1f(speedUniform,     speed);
    glUniform1f(thicknessUniform, thickness);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, additive ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLCircle2D::Destroy() {}
