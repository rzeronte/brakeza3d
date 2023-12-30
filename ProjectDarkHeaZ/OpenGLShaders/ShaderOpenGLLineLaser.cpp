//
// Created by edu on 28/12/23.
//

#include "ShaderOpenGLLineLaser.h"


#include <gtc/type_ptr.hpp>
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

    setVec2("lineStart", glm::vec2(normAx, normAy));
    setVec2("lineEnd", glm::vec2(normBx, normBy));
    setVec4("lineColor", glm::vec4(c.r, c.g, c.b, 1.0f));
    setFloat("weight", weight);
    setFloat("time", Brakeza3D::get()->getExecutionTime());

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

