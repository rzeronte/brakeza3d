//
// Created by Eduardo on 24/01/2025.
//

#include "ShaderOpenGLLine3D.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLLine3D::ShaderOpenGLLine3D()
:
    VertexArrayID(0),
    ShaderOpenGL(
EngineSetup::get()->SHADERS_FOLDER + "Line3D.vs",
EngineSetup::get()->SHADERS_FOLDER + "Line3D.fs"
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
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        nullptr
    );
    glBindVertexArray(0);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
}

void ShaderOpenGLLine3D::render(Vertex3D from, Vertex3D to, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);

    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4Uniform(matrixProjectionUniform, ProjectionMatrix);
    setMat4Uniform(matrixViewUniform, ViewMatrix);

    vertices[0] = from.toGLM();
    vertices[1] = to.toGLM();

    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(glm::vec3), &vertices[0]);

    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, (GLint) 2);
}


void ShaderOpenGLLine3D::destroy()
{
}
