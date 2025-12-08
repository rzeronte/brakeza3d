//
// Created by edu on 29/12/23.
//

#include "../include/OpenGL/ShaderOGLTint.h"
#include "../../include/Config.h"
#include "../../include/Components/ComponentsManager.h"

ShaderOGLTint::ShaderOGLTint()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Tint.vs",
        Config::get()->SHADERS_FOLDER + "Tint.fs",
        false
    )
{
    setupQuadUniforms(programID);

    colorUniform = glGetUniformLocation(programID, "color");
    alphaUniform = glGetUniformLocation(programID, "alpha");
}

void ShaderOGLTint::render(Color c, float alpha, GLuint fbo)
{
    ComponentsManager::get()->Render()->changeOpenGLFramebuffer(fbo);

    ComponentsManager::get()->Render()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    glUniform1f(alphaUniform, alpha);
    glUniform3fv(colorUniform, 1, &c.toGLM()[0]);

    drawQuad();
}

void ShaderOGLTint::destroy()
{
    resetQuadMatrix();
}