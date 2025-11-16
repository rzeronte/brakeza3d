#include "../../include/OpenGL/ShaderOpenGLPoints.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLPoints::ShaderOpenGLPoints()
:
    VertexArrayID(0),
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Points.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Points.fs",
        false
    )
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
}

void ShaderOpenGLPoints::renderMeshAnimation(Mesh3DAnimation *mesh, GLuint framebuffer)
{
    for (auto &m: mesh->meshes) {
        render(
            mesh,
            m.feedbackBuffer,
            m.vertices.size(),
            Color::green(),
            framebuffer
        );
    }
}

void ShaderOpenGLPoints::renderMesh(Mesh3D *mesh, GLuint framebuffer)
{
    for (auto &m: mesh->meshes) {
        render(
            mesh,
            m.vertexbuffer,
            m.vertices.size(),
            Color::green(),
            framebuffer
        );
    }
}

void ShaderOpenGLPoints::render(Mesh3D* m, GLint vertexbuffer, int numberPoints, Color c, GLuint framebuffer)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", m->getModelMatrix());

    setVec3("color", c.toGLM());

    setVAOAttributes(vertexbuffer);

    glDrawArrays(GL_POINTS, 0, (GLint) numberPoints);

    glDisableVertexAttribArray(0);
}

void ShaderOpenGLPoints::setVAOAttributes(GLint vertexbuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShaderOpenGLPoints::destroy() {

}
