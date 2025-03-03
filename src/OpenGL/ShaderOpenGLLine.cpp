//
// Created by edu on 10/12/23.
//

#include <glm/gtc/type_ptr.hpp>
#include "../../include/OpenGL/ShaderOpenGLLine.h"
#include "../../include/Render/Logging.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLLine::ShaderOpenGLLine()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Line.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Line.fs",
        false
    )
{
    setupQuadUniforms(programID);

    lineStartUniform = glGetUniformLocation(programID, "lineStart");
    lineEndUniform = glGetUniformLocation(programID, "lineEnd");
    lineColorUniform = glGetUniformLocation(programID, "lineColor");
    weightUniform = glGetUniformLocation(programID, "weight");
}

void ShaderOpenGLLine::render(Point2D a, Point2D b, Color c, float weight, GLuint framebuffer)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    const auto normAx = (float) ((float)a.y / (float) EngineSetup::get()->screenHeight);
    const auto normAy = (float) ((float)a.x / (float) EngineSetup::get()->screenWidth);

    const auto normBx = (float) ((float)b.y / (float) EngineSetup::get()->screenHeight);
    const auto normBy = (float) ((float)b.x / (float) EngineSetup::get()->screenWidth);

    setVec2Uniform(lineStartUniform, glm::vec2(normAx, normAy));
    setVec2Uniform(lineEndUniform, glm::vec2(normBx, normBy));
    setVec4Uniform(lineColorUniform, glm::vec4(c.r, c.g, c.b, 1.0f));
    setFloatUniform(weightUniform, weight);

    glDisable(GL_DEPTH_TEST);

    drawQuad();

    glEnable(GL_DEPTH_TEST);
}

void ShaderOpenGLLine::destroy() {

}
