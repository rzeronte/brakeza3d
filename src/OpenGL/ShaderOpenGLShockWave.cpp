#include "../../include/OpenGL/ShaderOpenGLShockWave.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLShockWave::ShaderOpenGLShockWave()
:
    timeLive(0),
    focalPointX(500),
    focalPointY(500),
    ShaderOpenGL(
            EngineSetup::get()->SHADERS_FOLDER + "ShockWave.vs",
            EngineSetup::get()->SHADERS_FOLDER + "ShockWave.fs"
    )
{
    setupQuadUniforms(programID);

    timeUniform = glGetUniformLocation(programID, "shockwaveTime");
    focalPointXUniform = glGetUniformLocation(programID, "shockwaveFocalPointX");
    focalPointYUniform = glGetUniformLocation(programID, "shockwaveFocalPointY");

    screenWidthUniform = glGetUniformLocation(programID, "screenWidth");
    screenHeightUniform = glGetUniformLocation(programID, "screenHeight");
    textureUniform = glGetUniformLocation(programID, "sceneTexture");
}

void ShaderOpenGLShockWave::render(GLuint textureID, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glUseProgram(programID);

    loadQuadMatrixUniforms();

    glUniform1f(timeUniform, timeLive);

    auto w = ComponentsManager::get()->getComponentWindow();

    const float posX = ((float) focalPointX / (float) w->getWidth());
    const float posY = ((float) focalPointY / (float) w->getHeight());

    glUniform1f(focalPointXUniform, posX);
    glUniform1f(focalPointYUniform, posY);

    auto window = ComponentsManager::get()->getComponentWindow();
    glUniform1i(screenWidthUniform, window->getWidth());
    glUniform1i(screenHeightUniform, window->getHeight());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureUniform, 0);

    drawQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOpenGLShockWave::destroy()
{
    resetQuadMatrix();
}

void ShaderOpenGLShockWave::setPoint(Point2D p)
{
    focalPointX = p.x;
    focalPointY = p.y;
}

void ShaderOpenGLShockWave::setTimeLive(float timeLive) {
    ShaderOpenGLShockWave::timeLive = timeLive;
}
