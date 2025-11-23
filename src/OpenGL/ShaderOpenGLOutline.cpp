
#include <glm/ext/matrix_clip_space.hpp>
#include "../../include/OpenGL/ShaderOpenGLOutline.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Objects/Image3D.h"

ShaderOpenGLOutline::ShaderOpenGLOutline()
:
    ShaderBaseOpenGL(
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

void ShaderOpenGLOutline::renderOutline(GLuint textureID, const Color &c, float borderThickness, GLuint fbo)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(fbo);
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

void ShaderOpenGLOutline::drawOutline(Mesh3D *m, const Color &c, float borderThickness, GLuint fbo)
{
    auto shaderColor = ComponentsManager::get()->getComponentRender()->getShaderOGLColor();

    for (const auto& mm : m->meshes) {
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

void ShaderOpenGLOutline::drawOutline(Mesh3DAnimation *m, Color c, float borderThickness, GLuint fbo)
{
    auto componentRender = ComponentsManager::get()->getComponentRender();
    auto shaderColor = componentRender->getShaderOGLColor();

    for (const auto& mm : m->meshes) {
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

void ShaderOpenGLOutline::drawOutlineImage3D(Image3D *i, const Color &c, float borderThickness, GLuint framebuffer)
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