
#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/OpenGL/ShaderOpenGLOutline.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Objects/Image3D.h"

ShaderOpenGLOutline::ShaderOpenGLOutline()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Outliner.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Outliner.fs",
        false
    )
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "image");
    lineColorUniform = glGetUniformLocation(programID, "lineColor");
    borderThicknessUniform = glGetUniformLocation(programID, "borderThickness");
}

void ShaderOpenGLOutline::render(GLint textureID, Color c, float borderThickness)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

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
    auto componentRender = ComponentsManager::get()->getComponentRender();
    auto componentWindow = ComponentsManager::get()->getComponentWindow();
    auto shaderColor = componentRender->getShaderOGLColor();

    for (const auto& mm : m->meshes) {
        shaderColor->render(
            m->getModelMatrix(),
            mm.vertexbuffer,
            mm.uvbuffer,
            mm.normalbuffer,
            (int) mm.vertices.size(),
            true,
            Color::white()
        );
    }

    componentRender->changeOpenGLFramebuffer(componentWindow->getSceneFramebuffer());

    render(shaderColor->textureColorbuffer, c, borderThickness);

    componentRender->changeOpenGLFramebuffer(0);
}

void ShaderOpenGLOutline::drawOutlineImage3D(Image3D *i, Color c, float borderThickness)
{
    auto componentRender = ComponentsManager::get()->getComponentRender();
    auto componentWindow = ComponentsManager::get()->getComponentWindow();
    auto shaderColor = componentRender->getShaderOGLColor();

    shaderColor->render(
        i->getModelMatrix(),
        i->vertexbuffer,
        i->uvbuffer,
        i->normalbuffer,
        (int) i->vertices.size(),
        true,
        Color::white()
    );

    componentRender->changeOpenGLFramebuffer(componentWindow->getSceneFramebuffer());

    render(shaderColor->textureColorbuffer, c, borderThickness);

    componentRender->changeOpenGLFramebuffer(0);
}