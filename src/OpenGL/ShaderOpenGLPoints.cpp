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

void ShaderOpenGLPoints::render(GLint particlesBuffer, int numberPoints, Color cf, Color ct)
{
    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    glm::mat4 ViewMatrix = ComponentsManager::get()->getComponentCamera()->getCamera()->getViewMatrix();
    glm::mat4 ProjectionMatrix = Camera3D::getProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", glm::mat4(1.0f));

    setVec3("colorTo", glm::vec3(cf.r, cf.g, cf.b));
    setVec3("colorFrom", glm::vec3(ct.r, ct.g, ct.b));

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
