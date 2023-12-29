//
// Created by edu on 10/12/23.
//

#include <gtc/type_ptr.hpp>
#include "../../include/OpenGL/ShaderOpenGLLine.h"
#include "../../include/Render/Logging.h"
#include "../../include/EngineSetup.h"

ShaderOpenGLLine::ShaderOpenGLLine()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Line.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Line.fs"
    )
{
    setupQuadUniforms(programID);
}

void ShaderOpenGLLine::render(Point2D a, Point2D b, Color c, float weight, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);

    loadQuadMatrixUniforms();

    const float normAx = (float) ((float)a.y / (float) EngineSetup::get()->screenHeight);
    const float normAy = (float) ((float)a.x / (float) EngineSetup::get()->screenWidth);

    const float normBx = (float) ((float)b.y / (float) EngineSetup::get()->screenHeight);
    const float normBy = (float) ((float)b.x / (float) EngineSetup::get()->screenWidth);

    setVec2("lineStart", glm::vec2(normAx, normAy));
    setVec2("lineEnd", glm::vec2(normBx, normBy));
    setVec4("lineColor", glm::vec4(c.r, c.g, c.b, 1.0f));
    setFloat("weight", weight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    drawQuad();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

void ShaderOpenGLLine::destroy() {

}
