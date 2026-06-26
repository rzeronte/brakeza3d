#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/OpenGL/ShaderOGLRect.h"
#include "../../include/Config.h"
#include "../../include/Components/Components.h"

ShaderOGLRect::ShaderOGLRect()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Image.vs",
        Config::get()->SHADERS_FOLDER + "Rect.fs",
        false
    )
{
}

void ShaderOGLRect::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    CreateQuadVBO();
    LoadUniforms();
}

void ShaderOGLRect::LoadUniforms()
{
    modelMatrixUniform      = glGetUniformLocation(programID, "model");
    projectionMatrixUniform = glGetUniformLocation(programID, "projection");
    rectColorUniform        = glGetUniformLocation(programID, "rectColor");
}

void ShaderOGLRect::CreateQuadVBO()
{
    float vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ShaderOGLRect::renderRect(int x, int y, int w, int h, int worldW, int worldH, const Color &c, GLuint fbo) const
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    glViewport(0, 0, worldW, worldH);
    glDisable(GL_DEPTH_TEST);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    glm::mat4 projection = glm::ortho(0.0f, (float)worldW, (float)worldH, 0.0f, -1.0f, 1.0f);
    glm::vec2 pos  = glm::vec2(x, y);
    glm::vec2 size = glm::vec2(w, h);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUniformMatrix4fv(modelMatrixUniform,      1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, &projection[0][0]);
    glUniform4f(rectColorUniform, c.r, c.g, c.b, c.a);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLRect::Destroy() {}
