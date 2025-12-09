//
// Created by Eduardo on 08/12/2025.
//

#include "../../include/OpenGL/ShaderOGLGrid.h"
#include "../../include/Config.h"
#include "../../include/Components/Components.h"

ShaderOGLGrid::ShaderOGLGrid()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Grid.vs",
        Config::get()->SHADERS_FOLDER + "Grid.fs",
        false
    ),
    color(Color::gray())
{
    setupQuadUniforms(programID);

    gridSizeUniform = glGetUniformLocation(programID, "gridSize");
    gridOpacityUniform = glGetUniformLocation(programID, "gridOpacity");
    gridColorUniform = glGetUniformLocation(programID, "gridColor");
}

void ShaderOGLGrid::render(GLuint fbo)
{
    Components::get()->Render()->changeOpenGLFramebuffer(fbo);
    Components::get()->Render()->changeOpenGLProgram(programID);

    auto camera = Components::get()->Camera()->getCamera();
    setMat4Uniform(modelMatrixUniform, camera->getGLMMat4ViewMatrix());
    setMat4Uniform(projectionMatrixUniform, camera->getGLMMat4ProjectionMatrix());

    // Parámetros de la rejilla
    setFloatUniform(gridSizeUniform, gridSize);
    setFloatUniform(gridOpacityUniform, opacity);
    setVec3Uniform(gridColorUniform, color.toGLM());

    // Habilitar blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    drawQuad();

    glDisable(GL_BLEND);
}

void ShaderOGLGrid::destroy()
{
}