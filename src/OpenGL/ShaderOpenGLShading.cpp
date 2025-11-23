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

void ShaderOpenGLShading::renderMesh(Mesh3D *mesh, bool useFeedbackBuffer, GLuint fbo)
{
    for (auto &m: mesh->meshes) {
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


void ShaderOpenGLShading::render(
    glm::mat4 modelMatrix,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int size,
    GLuint fbo
)
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

void ShaderOpenGLShading::destroy() {

}
