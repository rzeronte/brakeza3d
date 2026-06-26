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
    lineStartUniform  = glGetUniformLocation(programID, "lineStart");
    lineEndUniform    = glGetUniformLocation(programID, "lineEnd");
    lineColorUniform  = glGetUniformLocation(programID, "lineColor");
    weightUniform     = glGetUniformLocation(programID, "weight");
    resolutionUniform = glGetUniformLocation(programID, "resolution");
}

void ShaderOGLLine::render(Point2D a, Point2D b, Color c, float weight, GLuint fbo)
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);
    LoadQuadMatrixUniforms();

    const float w = (float)Config::get()->screenWidth;
    const float h = (float)Config::get()->screenHeight;

    setVec2Uniform(lineStartUniform,  glm::vec2((float)a.x / w, (float)a.y / h));
    setVec2Uniform(lineEndUniform,    glm::vec2((float)b.x / w, (float)b.y / h));
    setVec4Uniform(lineColorUniform,  glm::vec4(c.r, c.g, c.b, c.a));
    setFloatUniform(weightUniform,    weight);
    setVec2Uniform(resolutionUniform, glm::vec2(w, h));

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    DrawQuad();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void ShaderOGLLine::Destroy() {

}
