#include "../../include/OpenGL/ShaderOGLPoints.h"
#include "../../include/Components/Components.h"

ShaderOGLPoints::ShaderOGLPoints()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Points.vs",
        Config::get()->SHADERS_FOLDER + "Points.fs",
        false
    )
{
}

void ShaderOGLPoints::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    LoadUniforms();
}

void ShaderOGLPoints::LoadUniforms()
{
}

void ShaderOGLPoints::renderMeshAnimation(Mesh3DAnimation *mesh, GLuint fbo) const
{
    for (auto &m: mesh->getMeshData()) {
        render(
            mesh->getModelMatrix(),
            m.feedbackBuffer,
            m.vertices.size(),
            Color::green(),
            fbo
        );
    }
}

void ShaderOGLPoints::renderMesh(Mesh3D *mesh, bool useFeedbackBuffer, GLuint fbo)
{
    for (auto &m: mesh->getMeshData()) {
        render(
            mesh->getModelMatrix(),
            useFeedbackBuffer ? m.feedbackBuffer : m.vertexBuffer,
            m.vertices.size(),
            Color::green(),
            fbo
        );
    }
}

void ShaderOGLPoints::render(glm::mat4 modelMatrix, GLuint vertexBuffer, int numberPoints, Color c, GLuint fbo) const
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);

    Components::get()->Render()->ChangeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    auto camera = Components::get()->Camera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", modelMatrix);

    setVec3("color", c.toGLM());

    setVAOAttributes(vertexBuffer);

    glDrawArrays(GL_POINTS, 0, numberPoints);

    glDisableVertexAttribArray(0);
}

void ShaderOGLPoints::setVAOAttributes(GLuint vertexBuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShaderOGLPoints::Destroy() {

}
