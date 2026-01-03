//
// Created by edu on 10/12/23.
//

#include <glm/gtc/type_ptr.hpp>
#include "../../../include/OpenGL/Quad/ShaderOGLLine.h"
#include "../../../include/Misc/Logging.h"
#include "../../../include/Config.h"
#include "../../../include/Components/Components.h"

ShaderOGLLine::ShaderOGLLine()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Line.vs",
        Config::get()->SHADERS_FOLDER + "Line.fs",
        false
    )
{
}

void ShaderOGLLine::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateQuadVBO();
    SetupQuadUniforms(programID);
}

void ShaderOGLLine::LoadUniforms()
{
    lineStartUniform = glGetUniformLocation(programID, "lineStart");
    lineEndUniform = glGetUniformLocation(programID, "lineEnd");
    lineColorUniform = glGetUniformLocation(programID, "lineColor");
    weightUniform = glGetUniformLocation(programID, "weight");
}

void ShaderOGLLine::render(Point2D a, Point2D b, Color c, float weight, GLuint fbo)
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);

    Components::get()->Render()->ChangeOpenGLProgram(programID);

    LoadQuadMatrixUniforms();

    const auto normAx = (float) a.y / (float) Config::get()->screenHeight;
    const auto normAy = (float) a.x / (float) Config::get()->screenWidth;

    const auto normBx = (float) b.y / (float) Config::get()->screenHeight;
    const auto normBy = (float) b.x / (float) Config::get()->screenWidth;

    setVec2Uniform(lineStartUniform, glm::vec2(normAx, normAy));
    setVec2Uniform(lineEndUniform, glm::vec2(normBx, normBy));
    setVec4Uniform(lineColorUniform, glm::vec4(c.r, c.g, c.b, 1.0f));
    setFloatUniform(weightUniform, weight);

    glDisable(GL_DEPTH_TEST);

    DrawQuad();

    glEnable(GL_DEPTH_TEST);
}

void ShaderOGLLine::Destroy() {

}
