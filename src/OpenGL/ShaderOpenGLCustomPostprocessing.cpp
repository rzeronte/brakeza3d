//
// Created by Eduardo on 12/02/2025.
//

#include "../include/OpenGL/ShaderOpenGLCustomPostprocessing.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLCustomPostprocessing::ShaderOpenGLCustomPostprocessing(
    const std::string &label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename
    )
:
    ShaderOpenGLCustom(label, vertexFilename, fragmentFilename, ShaderCustomTypes::SHADER_POSTPROCESSING)
{

    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "sceneTexture");
}

GLuint ShaderOpenGLCustomPostprocessing::compile()
{
    setupQuadUniforms(programID);

    ShaderOpenGLCustom::compile();

    return programID;
}


void ShaderOpenGLCustomPostprocessing::render(GLuint framebuffer)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    resetNumberTextures();

    auto globalTexture = ComponentsManager::get()->getComponentWindow()->globalTexture;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, globalTexture);
    glUniform1i(textureUniform, 0);
    increaseNumberTextures();

    setDataTypesUniforms();

    drawQuad();
}

void ShaderOpenGLCustomPostprocessing::destroy()
{
    resetQuadMatrix();
}
