//
// Created by edu on 12/12/23.
//

#include "../../include/OpenGL/ShaderOpenGLWireframe.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLWireframe::ShaderOpenGLWireframe()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Wireframe.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Wireframe.fs"
    )
{
}

void ShaderOpenGLWireframe::render(
    glm::mat4 ModelMatrix,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    GLuint framebuffer
)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);

    glDisable(GL_BLEND);
    glLineWidth(1.0f);
    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", ModelMatrix);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    glDrawArrays(GL_LINES, 0, (GLint) size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

}

void ShaderOpenGLWireframe::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
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

void ShaderOpenGLWireframe::destroy() {

}
