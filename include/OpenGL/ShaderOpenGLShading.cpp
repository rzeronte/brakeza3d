//
// Created by edu on 12/12/23.
//

#include "ShaderOpenGLShading.h"
#include "../ComponentsManager.h"

ShaderOpenGLShading::ShaderOpenGLShading()
:
    ShaderOpenGL("../shaders/Shading.vertexshader","../shaders/Shading.fragmentshader")
{
}

void ShaderOpenGLShading::render(
        glm::mat4 ModelMatrix,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size
)
{
    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);

    glDisable(GL_BLEND);

    glm::mat4 ViewMatrix = ComponentsManager::get()->getComponentCamera()->getCamera()->getViewMatrix();
    glm::mat4 ProjectionMatrix = Camera3D::getProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", ModelMatrix);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    glDrawArrays(GL_TRIANGLES, 0, (GLint) size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glEnable(GL_BLEND);
}

void ShaderOpenGLShading::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
{
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
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            1,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
            2,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr
    );
}