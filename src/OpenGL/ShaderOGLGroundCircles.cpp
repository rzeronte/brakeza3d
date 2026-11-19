/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#include "../../include/OpenGL/ShaderOGLGroundCircles.h"

#include <glm/vec2.hpp>

#include "../../include/Config.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Profiler.h"

ShaderOGLGroundCircles::ShaderOGLGroundCircles()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "GroundCircles.vs",
        Config::get()->SHADERS_FOLDER + "GroundCircles.fs",
        false
    )
{
}

void ShaderOGLGroundCircles::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();

    // Quad unidad en XZ (y=0) con UV: x, y, z, u, v
    const float quad[6 * 5] = {
        -1, 0, -1,  0, 0,
         1, 0, -1,  1, 0,
         1, 0,  1,  1, 1,
        -1, 0, -1,  0, 0,
         1, 0,  1,  1, 1,
        -1, 0,  1,  0, 1,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &quadVbo);
    glGenBuffers(1, &instanceVbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    const GLsizei stride = STRIDE * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
    glVertexAttribDivisor(3, 1);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ShaderOGLGroundCircles::LoadUniforms()
{
    viewUniform       = glGetUniformLocation(programID, "view");
    projectionUniform = glGetUniformLocation(programID, "projection");
    gPositionUniform  = glGetUniformLocation(programID, "gPosition");
    screenSizeUniform = glGetUniformLocation(programID, "screenSize");
    filledUniform     = glGetUniformLocation(programID, "uFilled");
}

void ShaderOGLGroundCircles::draw(const float* data, size_t count, GLuint fbo, bool filled)
{
    if (count == 0 || data == nullptr) return;

    auto render = Components::get()->Render();
    auto camera = Components::get()->Camera();
    auto win    = Components::get()->Window();

    GLint prevProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    GLint prevFbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevFbo);

    render->ChangeOpenGLProgram(programID);
    render->ChangeOpenGLFramebuffer(fbo);

    // gPosition del G-Buffer en slot de textura 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, win->getGBuffer().positions);
    glUniform1i(gPositionUniform, 1);

    setMat4Uniform(viewUniform,       camera->getGLMMat4ViewMatrix());
    setMat4Uniform(projectionUniform, camera->getGLMMat4ProjectionMatrix());
    setVec2Uniform(screenSizeUniform, glm::vec2((float)win->getWidthRender(), (float)win->getHeightRender()));
    glUniform1i(filledUniform, filled ? 1 : 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    const GLsizeiptr bytes = (GLsizeiptr)(count * STRIDE * sizeof(float));
    if (count > instanceCapacity) {
        instanceCapacity = count + count / 2 + 16;   // holgura: la selección crece y decrece cada pocos frames
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(instanceCapacity * STRIDE * sizeof(float)), nullptr, GL_STREAM_DRAW);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, data);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)count);
    Profiler::get()->incrementDrawCall(GL_TRIANGLES, 6, (GLsizei)count, filled ? "GroundCircles (blobs)" : "GroundCircles");

    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    render->ChangeOpenGLProgram(prevProgram);
    render->ChangeOpenGLFramebuffer(prevFbo);
}

void ShaderOGLGroundCircles::Destroy()
{
    // VAO/VBO no dependen del tamaño de ventana
}
