//
// Created by Eduardo on 14/11/2025.
//

#include "../../include/OpenGL/ShaderOGLShadowPassDebugLight.h"
#include "../../include/ComponentsManager.h"

ShaderOGLShadowPassDebugLight::ShaderOGLShadowPassDebugLight()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "ShadowPassDebugLight.vs",
        EngineSetup::get()->SHADERS_FOLDER + "ShadowPassDebugLight.fs",
        false
    )
{
    setupQuadUniforms(programID);
}

void ShaderOGLShadowPassDebugLight::render(GLuint shadowMapArrayTex, int layer, GLuint framebuffer)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();
    setInt("layer", layer);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArrayTex);

    drawQuad();

    glBindVertexArray(0);
}

void ShaderOGLShadowPassDebugLight::destroy()
{
}
