#include "../../include/OpenGL/ShaderOGLAxisQuad.h"

#include <glm/gtc/matrix_transform.hpp>
#include "../../include/Config.h"
#include "../../include/Components/Components.h"
#include "../../include/3D/Object3D.h"

ShaderOGLAxisQuad::ShaderOGLAxisQuad()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "AxisQuad.vs",
        Config::get()->SHADERS_FOLDER + "AxisQuad.fs",
        false
    )
{
}

void ShaderOGLAxisQuad::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void ShaderOGLAxisQuad::LoadUniforms()
{
    modelUniform       = glGetUniformLocation(programID, "model");
    viewUniform        = glGetUniformLocation(programID, "view");
    projectionUniform  = glGetUniformLocation(programID, "projection");
    colorUniform       = glGetUniformLocation(programID, "quadColor");
}

void ShaderOGLAxisQuad::drawImpl(const glm::vec3& pos,
                                 const glm::mat4& rotationMatrix,
                                 const Color& color,
                                 float halfSize,
                                 GLuint fbo,
                                 Axis axis)
{
    auto render = Components::get()->Render();
    auto camera = Components::get()->Camera();

    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    GLint drawFboId = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);

    render->ChangeOpenGLProgram(programID);
    render->ChangeOpenGLFramebuffer(fbo);

    //------------------------------------------------------------
    // Modelo: T(pos) * R(obj) * axisRot * S(halfSize)
    //------------------------------------------------------------
    glm::mat4 axisRot(1.0f);
    switch (axis) {
        case AXIS_X:
            axisRot[0] = glm::vec4( 0, 0,-1, 0);
            axisRot[1] = glm::vec4( 0, 1, 0, 0);
            axisRot[2] = glm::vec4( 1, 0, 0, 0);
            break;
        case AXIS_Y:
            axisRot[0] = glm::vec4( 1, 0, 0, 0);
            axisRot[1] = glm::vec4( 0, 0, 1, 0);
            axisRot[2] = glm::vec4( 0,-1, 0, 0);
            break;
        case AXIS_Z:
            break;
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos)
                    * rotationMatrix
                    * axisRot
                    * glm::scale(glm::mat4(1.0f), glm::vec3(halfSize));

    //------------------------------------------------------------
    // Quad XY unitario (se rota vía axisRot en model)
    //------------------------------------------------------------
    const float verts[6][3] = {
        {-1,-1,0}, { 1,-1,0}, { 1, 1,0},
        {-1,-1,0}, { 1, 1,0}, {-1, 1,0}
    };

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    setMat4Uniform(modelUniform,       model);
    setMat4Uniform(viewUniform,        camera->getGLMMat4ViewMatrix());
    setMat4Uniform(projectionUniform,  camera->getGLMMat4ProjectionMatrix());
    setVec4Uniform(colorUniform,       glm::vec4(color.toGLM(), 1.0f));

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    render->ChangeOpenGLProgram(currentProgram);
    render->ChangeOpenGLFramebuffer(drawFboId);
}

void ShaderOGLAxisQuad::draw(Object3D* obj,
                             const Color& color,
                             float halfSize,
                             GLuint fbo,
                             Axis axis)
{
    drawImpl(
        obj->getPosition().toGLM(),
        glm::mat4(obj->getRotation().toGLMMat3()),
        color, halfSize, fbo, axis
    );
}

void ShaderOGLAxisQuad::drawAt(const Vertex3D& pos,
                               const Color& color,
                               float halfSize,
                               GLuint fbo,
                               Axis axis)
{
    drawImpl(
        pos.toGLM(),
        glm::mat4(1.0f),
        color, halfSize, fbo, axis
    );
}

void ShaderOGLAxisQuad::Destroy()
{
    // VAO/VBO are size-independent — nothing to destroy on resize
}
