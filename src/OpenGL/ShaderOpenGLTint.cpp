//
// Created by edu on 29/12/23.
//

#include "../include/OpenGL/ShaderOpenGLTint.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLTint::ShaderOpenGLTint()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Tint.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Tint.fs",
        false
    )
{
    setupQuadUniforms(programID);

    colorUniform = glGetUniformLocation(programID, "color");
    alphaUniform = glGetUniformLocation(programID, "alpha");
}

void ShaderOpenGLTint::render(Color c, float alpha, GLuint framebuffer)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    glUniform1f(alphaUniform, alpha);
    glUniform3fv(colorUniform, 1, &c.toGLM()[0]);

    drawQuad();
}

void ShaderOpenGLTint::destroy()
{
    resetQuadMatrix();
}