//
// Created by Eduardo on 24/01/2025.
//

#include "../include/OpenGL/ShaderOGLLine3D.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOGLLine3D::ShaderOGLLine3D()
:
    VertexArrayID(0),
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Line3D.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Line3D.fs",
        false
    )
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

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    colorUniform = glGetUniformLocation(programID, "color");
}

void ShaderOGLLine3D::render(Vertex3D from, Vertex3D to, GLuint fbo, Color c)
{
    const std::vector<Vector3D> vertices = {Vector3D(from, to)};

    renderLines(vertices, fbo, c);
}


void ShaderOGLLine3D::destroy()
{
}

void ShaderOGLLine3D::renderLines(const std::vector<Vector3D>& lines, GLuint fbo, Color c)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(fbo);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4Uniform(matrixProjectionUniform, ProjectionMatrix);
    setMat4Uniform(matrixViewUniform, ViewMatrix);
    setVec3Uniform(colorUniform, c.toGLM());

    std::vector<glm::vec3> vertices;

    for (auto &v: lines) {
        vertices.push_back(v.vertex1.toGLM());
        vertices.push_back(v.vertex2.toGLM());
    }

    GLuint VBO;
    glGenBuffers(1, &VBO);

    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, static_cast<GLint>(vertices.size()));

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(0);
}
