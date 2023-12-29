//
// Created by edu on 29/12/23.
//

#include "ShaderOpenGLTint.h"
#include "../../include/EngineSetup.h"

ShaderOpenGLTint::ShaderOpenGLTint()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Tint.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Tint.fs"
    )
{
    setupQuadUniforms(programID);

    colorUniform = glGetUniformLocation(programID, "color");
    alphaUniform = glGetUniformLocation(programID, "alpha");
}

void ShaderOpenGLTint::render(Color c, float alpha, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);

    loadQuadMatrixUniforms();

    glUniform1f(alphaUniform, alpha);
    glUniform3fv(colorUniform, 1, &c.toGLM()[0]);

    drawQuad();
}

void ShaderOpenGLTint::destroy()
{
    resetQuadMatrix();
}