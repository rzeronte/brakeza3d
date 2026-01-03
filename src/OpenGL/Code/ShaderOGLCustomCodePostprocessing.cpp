//
// Created by Eduardo on 12/02/2025.
//

#include "../../../include/OpenGL/Code/ShaderOGLCustomCodePostprocessing.h"
#include "../../../include/Components/Components.h"

ShaderOGLCustomCodePostprocessing::ShaderOGLCustomCodePostprocessing(
    const std::string &label,
    const std::string &typesFile,
    const std::string &vsFile,
    const std::string &fsFile
    )
:
    ShaderCustomOGLCode(label, typesFile, vsFile, fsFile, SHADER_POSTPROCESSING)
{
}

ShaderOGLCustomCodePostprocessing::ShaderOGLCustomCodePostprocessing(
    const std::string &label,
    const std::string &typesFile,
    const std::string &vsFile,
    const std::string &fsFile,
    cJSON *types
)
:
    ShaderCustomOGLCode(label, typesFile, vsFile, fsFile, SHADER_POSTPROCESSING, types)
{
}

void ShaderOGLCustomCodePostprocessing::LoadUniforms()
{
}

void ShaderOGLCustomCodePostprocessing::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();

    LoadUniforms();
    CreateQuadVBO();
    SetupQuadUniforms(programID);
}

void ShaderOGLCustomCodePostprocessing::render(GLuint fbo, GLuint texture)
{
    if (!isEnabled()) return;

    auto render = Components::get()->Render();
    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);

    LoadQuadMatrixUniforms();

    ResetNumberTextures();
    setTextureResult(texture);
    setDataTypesUniforms();

    DrawQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOGLCustomCodePostprocessing::Destroy()
{
    ShaderCustomOGLCode::Destroy();
    ResetQuadMatrix();
}

