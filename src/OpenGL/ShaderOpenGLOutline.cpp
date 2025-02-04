
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/OpenGL/ShaderOpenGLOutline.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLOutline::ShaderOpenGLOutline()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Outliner.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Outliner.fs"
    )
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "image");
    lineColorUniform = glGetUniformLocation(programID, "lineColor");
    borderThicknessUniform = glGetUniformLocation(programID, "borderThickness");
}

void ShaderOpenGLOutline::render(GLint textureID, Color c, float borderThickness)
{
    glUseProgram(programID);

    loadQuadMatrixUniforms();

    glUniform3fv(lineColorUniform, 1, &c.toGLM()[0]);
    glUniform1f(borderThicknessUniform, borderThickness);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureUniform, 0);

    drawQuad();
}

void ShaderOpenGLOutline::destroy()
{
    resetQuadMatrix();
}

void ShaderOpenGLOutline::drawOutline(Mesh3D *m, Color c, float borderThickness)
{
    auto componentWindow = ComponentsManager::get()->getComponentWindow();
    auto shaderColor = componentWindow->getShaderOGLColor();

    for (const auto& mm : m->meshes) {
        shaderColor->render(
            m,
            mm.vertexbuffer,
            mm.uvbuffer,
            mm.normalbuffer,
            (int) mm.vertices.size(),
            true,
            Color::white()
        );
    }

    glBindFramebuffer(GL_FRAMEBUFFER, componentWindow->getSceneFramebuffer());

    render(shaderColor->textureColorbuffer, c, borderThickness);

    glDeleteTextures(1, &shaderColor->textureColorbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}