/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by Eduardo on 24/01/2025.
//

#include "../include/OpenGL/ShaderOGLLine3D.h"
#include "../../include/Config.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Profiler.h"

ShaderOGLLine3D::ShaderOGLLine3D()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Line3D.vs",
        Config::get()->SHADERS_FOLDER + "Line3D.fs",
        false
    )
{
}

void ShaderOGLLine3D::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateLineVBO();
}

void ShaderOGLLine3D::LoadUniforms()
{
    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    colorUniform = glGetUniformLocation(programID, "color");
}

void ShaderOGLLine3D::render(Vertex3D from, Vertex3D to, GLuint fbo, Color c)
{
    const std::vector<Vector3D> vertices = {Vector3D(from, to)};

    renderLines(vertices, fbo, c);
}


void ShaderOGLLine3D::Destroy()
{
}

void ShaderOGLLine3D::renderLines(const std::vector<Vector3D>& lines, GLuint fbo, Color c)
{
    if (lines.empty()) return;

    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    // Viewport del DESTINO: el framebuffer de UI tiene el tamano de la VENTANA y el resto el de render. Con
    // resolucion de render bloqueada (RTS: 1600x900) distinta de la ventana, sin esto las lineas (debug de
    // fisicas, grids...) se dibujaban con el viewport de render pegadas a la esquina inferior izquierda y
    // no coincidian con la escena, que se estira a la ventana. Mismo criterio que Image y TextWriter.
    auto* window = Components::get()->Window();
    const bool toUI = (fbo == window->getUIFramebuffer());
    ComponentRender::ScopedTargetViewport viewport(
        toUI ? window->getWidth()  : window->getWidthRender(),
        toUI ? window->getHeight() : window->getHeightRender()
    );

    auto camera = Components::get()->Camera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4Uniform(matrixProjectionUniform, ProjectionMatrix);
    setMat4Uniform(matrixViewUniform, ViewMatrix);
    setVec4Uniform(colorUniform, glm::vec4(c.toGLM(), c.a));

    std::vector<glm::vec3> vertices;

    for (auto &v: lines) {
        vertices.push_back(v.vertex1.toGLM());
        vertices.push_back(v.vertex2.toGLM());
    }

    GLuint VBO;
    glGenBuffers(1, &VBO);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, static_cast<GLint>(vertices.size()));
    Profiler::get()->incrementDrawCall(GL_LINES, static_cast<GLint>(vertices.size()), 1, "Line3D");

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);

    glDeleteBuffers(1, &VBO);

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLLine3D::CreateLineVBO()
{
    glGenVertexArrays(1, &VertexArrayID);
    glGenBuffers(1, &vertexbuffer);

    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute
        4,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        nullptr
    );
    glBindVertexArray(0);
}