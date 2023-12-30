//
// Created by edu on 30/12/23.
//

#include "ShaderOpenGLImageOffset.h"
#include "../../include/Render/Logging.h"

ShaderOpenGLImageOffset::ShaderOpenGLImageOffset()
:
    ShaderOpenGL(
        "../ProjectDarkHeaZ/OpenGLShaders/ImageOffset.vs",
        "../ProjectDarkHeaZ/OpenGLShaders/ImageOffset.fs"
    )
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "image");
    alphaUniform = glGetUniformLocation(programID, "alpha");
    offsetUniform = glGetUniformLocation(programID, "offset");
    intensityUniform = glGetUniformLocation(programID, "intensity");
}

void ShaderOpenGLImageOffset::render(GLuint textureID, float offsetX, float offsetY, float intensity, float alpha, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);

    loadQuadMatrixUniforms();

    glm::vec2 offset(offsetY, offsetX);

    glUniform2fv(offsetUniform, 1, &offset[0]);
    glUniform1f(alphaUniform, alpha);
    glUniform1f(intensityUniform, intensity);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureUniform, 0);

    drawQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOpenGLImageOffset::destroy()
{
    resetQuadMatrix();
}
