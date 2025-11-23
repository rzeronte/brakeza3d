//
// Created by edu on 12/12/23.
//

#include "../../include/OpenGL/ShaderOGLWire.h"
#include "../../include/ComponentsManager.h"

ShaderOGLWire::ShaderOGLWire()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Wireframe.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Wireframe.fs",
        false
    )
{
}

void ShaderOGLWire::renderMesh(Mesh3D *mesh, bool useFeedbackFramebuffer, GLuint fbo) const
{
    for (auto &m: mesh->getMeshData()) {
        render(
            mesh->getModelMatrix(),
            useFeedbackFramebuffer ? m.feedbackBuffer : m.vertexBuffer,
            m.uvBuffer,
            m.normalBuffer,
            m.vertices.size(),
            fbo
        );
    }
}

void ShaderOGLWire::render(const glm::mat4 &modelMatrix, GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer, int size, GLuint fbo) const
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(fbo);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    glDisable(GL_BLEND);
    glLineWidth(1.0f);
    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", modelMatrix);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    glDrawArrays(GL_LINES, 0, size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void ShaderOGLWire::destroy() {
}
