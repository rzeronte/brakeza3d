//
// Created by Eduardo on 08/12/2025.
//

#include "../../../include/OpenGL/Quad/ShaderOGLGrid.h"
#include "../../../include/Config.h"
#include "../../../include/Components/Components.h"

ShaderOGLGrid::ShaderOGLGrid()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Grid.vs",
        Config::get()->SHADERS_FOLDER + "Grid.fs",
        false
    ),
    color(Color::gray())
{
}

void ShaderOGLGrid::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateQuadVBO();
    SetupQuadUniforms(programID);
}

void ShaderOGLGrid::LoadUniforms()
{

    gridSizeUniform = glGetUniformLocation(programID, "gridSize");
    gridOpacityUniform = glGetUniformLocation(programID, "gridOpacity");
    gridColorUniform = glGetUniformLocation(programID, "gridColor");
}

void ShaderOGLGrid::render(GLuint fbo)
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidthRender(), window->getHeightRender());

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

    DrawQuad();

    glDisable(GL_BLEND);
}

void ShaderOGLGrid::Destroy()
{
}