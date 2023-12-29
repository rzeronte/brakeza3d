#include "../../include/OpenGL/ShaderOpenGLShockWave.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLShockWave::ShaderOpenGLShockWave()
:
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

    speedUniform = glGetUniformLocation(programID, "speed");
}

void ShaderOpenGLShockWave::render(Point2D position, float timeLive, float speed, ShockWaveParams params, GLuint textureID, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glUseProgram(programID);

    loadQuadMatrixUniforms();

    glUniform1f(timeUniform, timeLive);

    const float posX = ((float) position.x / (float) EngineSetup::get()->screenWidth);
    const float posY = ((float) position.y / (float) EngineSetup::get()->screenHeight);

    glUniform1f(focalPointXUniform, posX);
    glUniform1f(focalPointYUniform, posY);

    glUniform1f(speedUniform, speed);

    auto window = ComponentsManager::get()->getComponentWindow();
    glUniform1i(screenWidthUniform, window->getWidth());
    glUniform1i(screenHeightUniform, window->getHeight());

    setFloat("shockParams.x", params.param1);
    setFloat("shockParams.y", params.param2);
    setFloat("shockParams.z", params.param3);

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
