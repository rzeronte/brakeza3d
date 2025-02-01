#include "../../include/OpenGL/ShaderOpenGLPoints.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLPoints::ShaderOpenGLPoints()
:
    VertexArrayID(0),
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Points.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Points.fs"
    )
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
}

void ShaderOpenGLPoints::render(GLint particlesBuffer, int numberPoints, Color cf, Color ct, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    auto camera = ComponentsManager::get()->getComponentCamera();
    glm::mat4 ViewMatrix = camera->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = camera->getGLMMat4ProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", glm::mat4(1.0f));

    setVec3("colorTo", cf.toGLM());
    setVec3("colorFrom", ct.toGLM());

    setVAOAttributes(particlesBuffer);

    glDrawArrays(GL_POINTS, 0, (GLint) numberPoints);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

void ShaderOpenGLPoints::setVAOAttributes(GLint particlesBuffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, particlesBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(OCParticle), (GLvoid*)offsetof(OCParticle, position.x));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(OCParticle), (GLvoid*)offsetof(OCParticle, active));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(OCParticle), (GLvoid*)offsetof(OCParticle, timeToLive));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(OCParticle), (GLvoid*)offsetof(OCParticle, timeLiving));
}

void ShaderOpenGLPoints::destroy() {

}
