
#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/OpenGL/ShaderOGLOutline.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Objects/Image3D.h"

ShaderOGLOutline::ShaderOGLOutline()
:
    ShaderBaseOpenGL(
        BrakezaSetup::get()->SHADERS_FOLDER + "Outliner.vs",
        BrakezaSetup::get()->SHADERS_FOLDER + "Outliner.fs",
        false
    )
{
    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "image");
    lineColorUniform = glGetUniformLocation(programID, "lineColor");
    borderThicknessUniform = glGetUniformLocation(programID, "borderThickness");
}

void ShaderOGLOutline::renderOutline(GLuint textureId, const Color &c, float borderThickness, GLuint fbo)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(fbo);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    glUniform3fv(lineColorUniform, 1, &c.toGLM()[0]);
    glUniform1f(borderThicknessUniform, borderThickness);

    setTextureUniform(textureUniform, textureId, 0);

    drawQuad();
}

void ShaderOGLOutline::destroy()
{
    resetQuadMatrix();
}

void ShaderOGLOutline::drawOutline(Mesh3D *m, const Color &c, float borderThickness, GLuint fbo)
{
    auto shaderColor = ComponentsManager::get()->getComponentRender()->getShaderOGLColor();

    for (const auto& mm : m->getMeshData()) {
        shaderColor->renderColor(
            m->getModelMatrix(),
            mm.vertexBuffer,
            mm.uvBuffer,
            mm.normalBuffer,
            static_cast<int>(mm.vertices.size()),
            Color::white(),
            true,
            shaderColor->getFramebuffer()
        );
    }

    renderOutline(shaderColor->getTextureColorBuffer(), c, borderThickness, fbo);
}

void ShaderOGLOutline::drawOutline(Mesh3DAnimation *m, Color c, float borderThickness, GLuint fbo)
{
    auto componentRender = ComponentsManager::get()->getComponentRender();
    auto shaderColor = componentRender->getShaderOGLColor();

    for (const auto& mm : m->getMeshData()) {
        shaderColor->renderColor(
            m->getModelMatrix(),
            mm.feedbackBuffer,
            mm.uvBuffer,
            mm.normalBuffer,
            static_cast<int>(mm.vertices.size()),
            Color::white(),
            true,
            shaderColor->getFramebuffer()
        );
    }

    renderOutline(shaderColor->getTextureColorBuffer(), c, borderThickness, fbo);
}

void ShaderOGLOutline::drawOutlineImage3D(Image3D *i, const Color &c, float borderThickness, GLuint framebuffer)
{
    auto shaderColor = ComponentsManager::get()->getComponentRender()->getShaderOGLColor();

    shaderColor->renderColor(
        i->getModelMatrix(),
        i->getVertexBuffer(),
        i->getUVBuffer(),
        i->getNormalBuffer(),
        static_cast<int>(i->getVertices().size()),
        Color::white(),
        true,
        shaderColor->getFramebuffer()
    );

    renderOutline(shaderColor->getTextureColorBuffer(), c, borderThickness, framebuffer);
}