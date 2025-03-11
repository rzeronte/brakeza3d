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
}

ShaderOpenGLCustomPostprocessing::ShaderOpenGLCustomPostprocessing(
    const std::string &label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename,
    cJSON *types
)
:
    ShaderOpenGLCustom(label, vertexFilename, fragmentFilename, ShaderCustomTypes::SHADER_POSTPROCESSING, types)
{
    setupQuadUniforms(programID);
}

GLuint ShaderOpenGLCustomPostprocessing::compile()
{
    setupQuadUniforms(programID);

    ShaderOpenGLCustom::compile();

    return programID;
}

void ShaderOpenGLCustomPostprocessing::render(GLuint framebuffer)
{
    if (!isEnabled()) return;

    auto render = ComponentsManager::get()->getComponentRender();
    render->changeOpenGLFramebuffer(framebuffer);
    render->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    resetNumberTextures();
    setDataTypesUniforms();

    drawQuad();
}

void ShaderOpenGLCustomPostprocessing::destroy()
{
    ShaderOpenGLCustom::destroy();
    resetQuadMatrix();
}
