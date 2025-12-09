//
// Created by Eduardo on 12/02/2025.
//

#include "../include/OpenGL/ShaderOGLCustomPostprocessing.h"
#include "../../include/Components/Components.h"

ShaderOGLCustomPostprocessing::ShaderOGLCustomPostprocessing(
    const std::string &label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename
    )
:
    ShaderOGLCustom(label, vertexFilename, fragmentFilename, ShaderCustomType::SHADER_POSTPROCESSING)
{
    setupQuadUniforms(programID);
}

ShaderOGLCustomPostprocessing::ShaderOGLCustomPostprocessing(
    const std::string &label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename,
    cJSON *types
)
:
    ShaderOGLCustom(label, vertexFilename, fragmentFilename, ShaderCustomType::SHADER_POSTPROCESSING, types)
{
    setupQuadUniforms(programID);
}

GLuint ShaderOGLCustomPostprocessing::compile()
{
    setupQuadUniforms(programID);

    ShaderOGLCustom::compile();

    return programID;
}

void ShaderOGLCustomPostprocessing::render(GLuint fbo)
{
    if (!isEnabled()) return;

    auto render = Components::get()->Render();
    render->changeOpenGLFramebuffer(fbo);
    render->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    ResetNumberTextures();
    setDataTypesUniforms();

    drawQuad();
}

void ShaderOGLCustomPostprocessing::destroy()
{
    ShaderOGLCustom::destroy();
    resetQuadMatrix();
}
