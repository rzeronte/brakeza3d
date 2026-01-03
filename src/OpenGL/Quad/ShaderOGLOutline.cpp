
#include <glm/ext/matrix_clip_space.hpp>
#include "../../../include/OpenGL/Quad/ShaderOGLOutline.h"
#include "../../../include/Components/Components.h"
#include "../../../include/3D/Image3D.h"

ShaderOGLOutline::ShaderOGLOutline()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Outliner.vs",
        Config::get()->SHADERS_FOLDER + "Outliner.fs",
        false
    )
{
}

void ShaderOGLOutline::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateQuadVBO();
    SetupQuadUniforms(programID);
}

void ShaderOGLOutline::LoadUniforms()
{
    textureUniform = glGetUniformLocation(programID, "image");
    lineColorUniform = glGetUniformLocation(programID, "lineColor");
    borderThicknessUniform = glGetUniformLocation(programID, "borderThickness");
}

void ShaderOGLOutline::renderOutline(GLuint textureId, const Color &c, float borderThickness, GLuint fbo)
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidth(), window->getHeight());

    LoadQuadMatrixUniforms();

    glUniform3fv(lineColorUniform, 1, &c.toGLM()[0]);
    glUniform1f(borderThicknessUniform, borderThickness);

    setTextureUniform(textureUniform, textureId, 0);

    DrawQuad();
}

void ShaderOGLOutline::Destroy()
{
    ResetQuadMatrix();
}

void ShaderOGLOutline::drawOutline(Mesh3D *m, const Color &c, float borderThickness, GLuint fbo)
{
    auto shaderColor = Components::get()->Render()->getShaders()->shaderOGLColor;

    Components::get()->Render()->ChangeOpenGLFramebuffer(shaderColor->getFramebuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& mm : m->getMeshData()) {
        shaderColor->RenderColor(
            m->getModelMatrix(),
            mm.vertexBuffer,
            mm.uvBuffer,
            mm.normalBuffer,
            (int) mm.vertices.size(),
            Color::white(),
            false,
            shaderColor->getFramebuffer()
        );
    }

    renderOutline(shaderColor->getTextureColorBuffer(), c, borderThickness, fbo);
}

void ShaderOGLOutline::drawOutline(Mesh3DAnimation *m, Color c, float borderThickness, GLuint fbo)
{
    auto componentRender = Components::get()->Render();
    auto shaderColor = componentRender->getShaders()->shaderOGLColor;

    for (const auto& mm : m->getMeshData()) {
        shaderColor->RenderColor(
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
    auto shaderColor = Components::get()->Render()->getShaders()->shaderOGLColor;

    shaderColor->RenderColor(
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