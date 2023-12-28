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
    quadVAO(0),
    VBO(0),
    ShaderOpenGL(
    "../ProjectDarkHeaZ/OpenGLShaders/LineLaser.vertexshader",
"../ProjectDarkHeaZ/OpenGLShaders/LineLaser.fragmentshader"
     )
{
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    image = new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");

    modelMatrixUniform = glGetUniformLocation(programID, "model");
    projectionMatrixUniform = glGetUniformLocation(programID, "projection");
    textureUniform = glGetUniformLocation(programID, "mask");
}

void ShaderOpenGLLineLaser::render(Point2D a, Point2D b, Color c, float weight, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);
    glBindVertexArray(quadVAO);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    auto window = ComponentsManager::get()->getComponentWindow();

    const glm::mat4 projection = glm::ortho(0.0f, (float) window->width, (float) window->height, 0.0f, -1.0f, 1.0f);
    const glm::vec2 size = glm::vec2((float) window->width, (float) window->height);
    float rotate = 0.0f;

    glm::mat4 model = glm::transpose(glm::mat4(1.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &projection[0][0]);

    const float normAx = (float) ((float)a.y / (float) EngineSetup::get()->screenHeight);
    const float normAy = (float) ((float)a.x / (float) EngineSetup::get()->screenWidth);

    const float normBx = (float) ((float)b.y / (float) EngineSetup::get()->screenHeight);
    const float normBy = (float) ((float)b.x / (float) EngineSetup::get()->screenWidth);

    setVec2("lineStart", glm::vec2(normAx, normAy));
    setVec2("lineEnd", glm::vec2(normBx, normBy));
    setVec4("lineColor", glm::vec4(c.r, c.g, c.b, 1.0f));
    setFloat("weight", weight);
    setFloat("time", Brakeza3D::get()->getExecutionTime());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->getOGLTextureID());
    glUniform1i(textureUniform, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_DEPTH_TEST);

    glDepthMask(GL_TRUE);

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void ShaderOpenGLLineLaser::destroy() {

}

