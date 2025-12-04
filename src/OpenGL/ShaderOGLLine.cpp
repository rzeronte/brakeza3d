//
// Created by edu on 10/12/23.
//

#include <glm/gtc/type_ptr.hpp>
#include "../../include/OpenGL/ShaderOGLLine.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Config.h"
#include "../../include/Components/ComponentsManager.h"

ShaderOGLLine::ShaderOGLLine()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Line.vs",
        Config::get()->SHADERS_FOLDER + "Line.fs",
        false
    )
{
    setupQuadUniforms(programID);

    lineStartUniform = glGetUniformLocation(programID, "lineStart");
    lineEndUniform = glGetUniformLocation(programID, "lineEnd");
    lineColorUniform = glGetUniformLocation(programID, "lineColor");
    weightUniform = glGetUniformLocation(programID, "weight");
}

void ShaderOGLLine::render(Point2D a, Point2D b, Color c, float weight, GLuint fbo)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(fbo);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    const auto normAx = (float) ((float)a.y / (float) Config::get()->screenHeight);
    const auto normAy = (float) ((float)a.x / (float) Config::get()->screenWidth);

    const auto normBx = (float) ((float)b.y / (float) Config::get()->screenHeight);
    const auto normBy = (float) ((float)b.x / (float) Config::get()->screenWidth);

    setVec2Uniform(lineStartUniform, glm::vec2(normAx, normAy));
    setVec2Uniform(lineEndUniform, glm::vec2(normBx, normBy));
    setVec4Uniform(lineColorUniform, glm::vec4(c.r, c.g, c.b, 1.0f));
    setFloatUniform(weightUniform, weight);

    glDisable(GL_DEPTH_TEST);

    drawQuad();

    glEnable(GL_DEPTH_TEST);
}

void ShaderOGLLine::destroy() {

}
