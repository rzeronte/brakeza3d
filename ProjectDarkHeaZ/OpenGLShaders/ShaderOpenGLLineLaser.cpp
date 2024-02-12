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
    ShaderOpenGL(
        "../GLSL/LineLaser.vs",
        "../GLSL/LineLaser.fs"
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

void ShaderOpenGLLineLaser::render(
    GLuint textureMaskID,
    Point2D a,
    Point2D b,
    glm::vec2 direction,
    Color c,
    float speed,
    float weight,
    float intensity,
    GLuint framebuffer
) {
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

    auto start = glm::vec2(normAx, normAy);
    auto end = glm::vec2(normBx, normBy);

    setVec2Uniform(lineStartUniform, start);
    setVec2Uniform(lineEndUniform, end);
    setVec4Uniform(lineColorUniform, glm::vec4(c.r, c.g, c.b, 1.0f));
    setFloatUniform(weightUniform, weight);
    setFloatUniform(timeUniform, Brakeza3D::get()->getExecutionTime());
    setVec2Uniform(maskDirectionUniform, direction);
    setFloatUniform(maskSpeedUniform, speed);
    setFloatUniform(maskIntensityUniform, intensity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMaskID);
    glUniform1i(textureUniform, 0);

    drawQuad();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOpenGLLineLaser::destroy() {

}

