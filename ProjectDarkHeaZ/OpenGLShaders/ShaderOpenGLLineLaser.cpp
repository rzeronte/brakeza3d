//
// Created by edu on 28/12/23.
//

#include "ShaderOpenGLLineLaser.h"

#include "../../include/OpenGL/ShaderOpenGLLine.h"
#include "../../include/Render/Logging.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ShaderOpenGLLineLaser::ShaderOpenGLLineLaser()
:
    image(new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png")),
    ShaderOpenGL(
    "../ProjectDarkHeaZ/OpenGLShaders/LineLaser.vertexshader",
"../ProjectDarkHeaZ/OpenGLShaders/LineLaser.fragmentshader"
     )
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "mask");
    maskDirectionUniform = glGetUniformLocation(programID, "maskDirection");
    lineStartUniform = glGetUniformLocation(programID, "lineStart");
    lineEndUniform = glGetUniformLocation(programID, "lineEnd");
    lineColorUniform = glGetUniformLocation(programID, "lineColor");
    weightUniform = glGetUniformLocation(programID, "weight");
    timeUniform = glGetUniformLocation(programID, "time");
    maskSpeedUniform = glGetUniformLocation(programID, "maskSpeed");
    maskIntensityUniform = glGetUniformLocation(programID, "maskIntensity");
}

void ShaderOpenGLLineLaser::render(Point2D a, Point2D b, Color c, float weight, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    loadQuadMatrixUniforms();

    const auto normAx = (float) ((float)a.y / (float) EngineSetup::get()->screenHeight);
    const auto normAy = (float) ((float)a.x / (float) EngineSetup::get()->screenWidth);

    const auto normBx = (float) ((float)b.y / (float) EngineSetup::get()->screenHeight);
    const auto normBy = (float) ((float)b.x / (float) EngineSetup::get()->screenWidth);

    setVec2Uniform(lineStartUniform, glm::vec2(normAx, normAy));
    setVec2Uniform(lineEndUniform, glm::vec2(normBx, normBy));
    setVec4Uniform(lineColorUniform, glm::vec4(c.r, c.g, c.b, 1.0f));
    setFloatUniform(weightUniform, weight);
    setFloatUniform(timeUniform, Brakeza3D::get()->getExecutionTime());
    setVec2Uniform(maskDirectionUniform, glm::vec2(0, 1 ));
    setFloatUniform(maskSpeedUniform, 0.5f);
    setFloatUniform(maskIntensityUniform, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->getOGLTextureID());
    glUniform1i(textureUniform, 0);

    drawQuad();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOpenGLLineLaser::destroy() {

}

