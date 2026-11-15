/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
    gridSizeUniform    = glGetUniformLocation(programID, "gridSize");
    gridOpacityUniform = glGetUniformLocation(programID, "gridOpacity");
    gridColorUniform   = glGetUniformLocation(programID, "gridColor");
    cameraPosUniform   = glGetUniformLocation(programID, "cameraPos");
    fadeStartUniform   = glGetUniformLocation(programID, "fadeStart");
    fadeEndUniform     = glGetUniformLocation(programID, "fadeEnd");
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

    setFloatUniform(gridSizeUniform, gridSize);
    setFloatUniform(gridOpacityUniform, opacity);
    setVec3Uniform(gridColorUniform, color.toGLM());
    setVec3Uniform(cameraPosUniform, camera->getPosition().toGLM());
    setFloatUniform(fadeStartUniform, fadeStart);
    setFloatUniform(fadeEndUniform, fadeEnd);

    // Habilitar blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    DrawQuad();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void ShaderOGLGrid::Destroy()
{
}