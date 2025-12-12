//
// Created by edu on 12/12/23.
//

#include "../../include/OpenGL/ShaderOGLWire.h"
#include "../../include/Components/Components.h"

ShaderOGLWire::ShaderOGLWire()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Wireframe.vs",
        Config::get()->SHADERS_FOLDER + "Wireframe.fs",
        false
    )
{
    colorUniform = glGetUniformLocation(programID, "color");
}

void ShaderOGLWire::renderMesh(Mesh3D *mesh, bool useFeedbackFramebuffer, const Color &c, GLuint fbo) const
{
    for (auto &m: mesh->getMeshData()) {
        render(
            mesh->getModelMatrix(),
            useFeedbackFramebuffer ? m.feedbackBuffer : m.vertexBuffer,
            m.uvBuffer,
            m.normalBuffer,
            m.vertices.size(),
            c,
            fbo
        );
    }
}

void ShaderOGLWire::render(
    const glm::mat4 &modelMatrix,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int size,
    const Color &c,
    GLuint fbo
) const
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    glDisable(GL_BLEND);
    glLineWidth(1.0f);
    auto camera = Components::get()->Camera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", modelMatrix);

    setVec3("color", c.toGLM());
    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    glDrawArrays(GL_LINES, 0, size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void ShaderOGLWire::destroy() {
}
