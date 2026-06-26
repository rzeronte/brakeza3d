#include "../../include/OpenGL/ShaderOGLGroundCircle.h"

#include <glm/ext/matrix_transform.hpp>

#include "../../include/Config.h"
#include "../../include/Components/Components.h"
#include "../../include/3D/Object3D.h"

struct GCVertex { float x, y, z, u, v; };

ShaderOGLGroundCircle::ShaderOGLGroundCircle()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "GroundCircle.vs",
        Config::get()->SHADERS_FOLDER + "GroundCircle.fs",
        false
    )
{
}

void ShaderOGLGroundCircle::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GCVertex), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GCVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GCVertex), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}

void ShaderOGLGroundCircle::LoadUniforms()
{
    modelUniform        = glGetUniformLocation(programID, "model");
    viewUniform         = glGetUniformLocation(programID, "view");
    projectionUniform   = glGetUniformLocation(programID, "projection");
    colorUniform        = glGetUniformLocation(programID, "circleColor");
    thicknessUniform    = glGetUniformLocation(programID, "thickness");
    gPositionUniform    = glGetUniformLocation(programID, "gPosition");
    screenSizeUniform   = glGetUniformLocation(programID, "screenSize");
    circleWorldYUniform = glGetUniformLocation(programID, "circleWorldY");
    filledUniform       = glGetUniformLocation(programID, "uFilled");
}

void ShaderOGLGroundCircle::draw(Object3D* obj, const Color& color, float radius, GLuint fbo, float thickness, bool filled)
{
    auto render = Components::get()->Render();
    auto camera = Components::get()->Camera();

    GLint prevProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);
    GLint prevFbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevFbo);

    render->ChangeOpenGLProgram(programID);
    render->ChangeOpenGLFramebuffer(fbo);

    // Local-space unit quad in XZ plane (y=0) with UVs for circle alpha
    GCVertex verts[6] = {
        {-1, 0, -1,  0, 0},
        { 1, 0, -1,  1, 0},
        { 1, 0,  1,  1, 1},
        {-1, 0, -1,  0, 0},
        { 1, 0,  1,  1, 1},
        {-1, 0,  1,  0, 1},
    };

    glm::vec3 pos = obj->getPosition().toGLM();
    pos.y += 0.05f;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos)
                    * glm::scale(glm::mat4(1.0f), glm::vec3(radius));

    auto win = Components::get()->Window();
    float screenW = (float)win->getWidthRender();
    float screenH = (float)win->getHeightRender();

    // gPosition del G-Buffer en slot de textura 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, win->getGBuffer().positions);
    glUniform1i(gPositionUniform, 1);

    setMat4Uniform(modelUniform,        model);
    setMat4Uniform(viewUniform,         camera->getGLMMat4ViewMatrix());
    setMat4Uniform(projectionUniform,   camera->getGLMMat4ProjectionMatrix());
    setVec4Uniform(colorUniform,        glm::vec4(color.toGLM(), 1.0f));
    setFloatUniform(thicknessUniform,    thickness);
    setVec2Uniform(screenSizeUniform,    glm::vec2(screenW, screenH));
    setFloatUniform(circleWorldYUniform, pos.y);
    glUniform1i(filledUniform, filled ? 1 : 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    render->ChangeOpenGLProgram(prevProgram);
    render->ChangeOpenGLFramebuffer(prevFbo);
}

void ShaderOGLGroundCircle::Destroy()
{
    // VAO/VBO are size-independent — nothing to destroy on resize
}
