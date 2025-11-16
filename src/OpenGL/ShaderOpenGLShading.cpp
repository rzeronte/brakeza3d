//
// Created by edu on 12/12/23.
//

#include "../../include/OpenGL/ShaderOpenGLShading.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLShading::ShaderOpenGLShading()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Shading.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Shading.fs",
        false
    )
{
}

void ShaderOpenGLShading::renderMesh(Mesh3D *mesh, GLuint framebuffer)
{
    auto window = ComponentsManager::get()->getComponentWindow();

    for (auto &m: mesh->meshes) {
        render(
            mesh->getModelMatrix(),
            m.vertexbuffer,
            m.uvbuffer,
            m.normalbuffer,
            (int) m.vertices.size(),
            window->getSceneFramebuffer()
        );
    }
}

void ShaderOpenGLShading::renderMeshAnimation(Mesh3D *mesh, GLuint framebuffer)
{
    for (auto &m: mesh->meshes) {
        render(
            mesh->getModelMatrix(),
            m.feedbackBuffer,
            m.uvbuffer,
            m.normalbuffer,
            (int) m.vertices.size(),
            framebuffer
        );
    }
}

void ShaderOpenGLShading::render(
    glm::mat4 ModelMatrix,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    GLuint framebuffer
)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    glDisable(GL_BLEND);

    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

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
            4,                  // size
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

void ShaderOpenGLShading::destroy() {

}
