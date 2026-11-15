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
// Created by edu on 29/12/23.
//

#include "../../../include/OpenGL/Quad/ShaderOGLTint.h"
#include "../../../include/Config.h"
#include "../../../include/Components/Components.h"

ShaderOGLTint::ShaderOGLTint()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Tint.vs",
        Config::get()->SHADERS_FOLDER + "Tint.fs",
        false
    )
{
}

void ShaderOGLTint::LoadUniforms()
{
    colorUniform = glGetUniformLocation(programID, "color");
    alphaUniform = glGetUniformLocation(programID, "alpha");
}

void ShaderOGLTint::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateQuadVBO();
    SetupQuadUniforms(programID);
}

void ShaderOGLTint::render(Color c, float alpha, GLuint fbo)
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);

    Components::get()->Render()->ChangeOpenGLProgram(programID);

    LoadQuadMatrixUniforms();

    glUniform1f(alphaUniform, alpha);
    glUniform3fv(colorUniform, 1, &c.toGLM()[0]);

    DrawQuad();
}

void ShaderOGLTint::Destroy()
{
    ResetQuadMatrix();
}