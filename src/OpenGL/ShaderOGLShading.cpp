//
// Created by edu on 12/12/23.
//

#include "../../include/OpenGL/ShaderOGLShading.h"
#include "../../include/Components/ComponentsManager.h"

ShaderOGLShading::ShaderOGLShading()
:
    ShaderBaseOpenGL(
        BrakezaSetup::get()->SHADERS_FOLDER + "Shading.vs",
        BrakezaSetup::get()->SHADERS_FOLDER + "Shading.fs",
        false
    )
{
}

void ShaderOGLShading::renderMesh(Mesh3D *mesh, bool useFeedbackBuffer, GLuint fbo)
{
    for (auto &m: mesh->getMeshData()) {
        render(
            mesh->getModelMatrix(),
            useFeedbackBuffer ? m.feedbackBuffer : m.vertexBuffer,
            m.uvBuffer,
            m.normalBuffer,
            m.vertices.size(),
            fbo
        );
    }
}

void ShaderOGLShading::render(glm::mat4 modelMatrix, GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer, int size,GLuint fbo) const
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(fbo);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    glDisable(GL_BLEND);

    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", modelMatrix);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    glDrawArrays(GL_TRIANGLES, 0, (GLint) size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glEnable(GL_BLEND);
}

void ShaderOGLShading::destroy() {

}
