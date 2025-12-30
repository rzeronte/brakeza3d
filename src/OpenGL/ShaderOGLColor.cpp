
//
// Created by edu on 17/12/23.
//

#include "../../include/OpenGL/ShaderOGLColor.h"
#include "../../include/Components/Components.h"

ShaderOGLColor::ShaderOGLColor()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Color.vs",
        Config::get()->SHADERS_FOLDER + "Color.fs",
        false
    )
{
}

void ShaderOGLColor::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    LoadUniforms();
    CreateBuffer();
}

void ShaderOGLColor::LoadUniforms()
{
}

void ShaderOGLColor::renderMesh(Mesh3D* m, bool useFeedbackBuffer, const Color &color, bool clearFramebuffer, GLuint fbo) const
{
    for (const auto& mm : m->getMeshData()) {
        RenderColor(
            m->getModelMatrix(),
            useFeedbackBuffer ? mm.feedbackBuffer : mm.vertexBuffer,
            mm.uvBuffer,
            mm.normalBuffer,
            static_cast<int>(mm.vertices.size()),
            color,
            clearFramebuffer,
            fbo
        );
    }
}

void ShaderOGLColor::RenderColor(
    const glm::mat4 &modelView,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int size,
    const Color &c,
    bool clearFBO,
    GLuint fbo
) const
{
    auto render = Components::get()->Render();
    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);
    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidthRender(), window->getHeightRender());

    glBindVertexArray(VertexArrayID);

    if (clearFBO) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    auto camera = Components::get()->Camera();

    setMat4("projection", camera->getGLMMat4ProjectionMatrix());
    setMat4("view", camera->getGLMMat4ViewMatrix());
    setMat4("model", modelView);
    setVec3("color", c.toGLM());

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    glDrawArrays(GL_TRIANGLES, 0,  size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    render->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLColor::Destroy()
{
    CreateBuffer();
}

void ShaderOGLColor::DeleteTexture() const
{
    glDeleteTextures(1, &textureColorBuffer);
}

GLuint ShaderOGLColor::getTextureColorBuffer() const
{
    return textureColorBuffer;
}

void ShaderOGLColor::CreateBuffer()
{
    if (framebuffer != 0) {
        glDeleteFramebuffers(1, &framebuffer);
    }
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    const int w = Components::get()->Window()->getWidthRender();
    const int h = Components::get()->Window()->getHeightRender();

    if (textureColorBuffer != 0) {
        glDeleteTextures(1, &textureColorBuffer);
        textureColorBuffer = 0;
    }

    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    // --- Depth buffer ---
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FBO INCOMPLETE: " << std::hex << status << std::dec << std::endl;
        exit(-1);
    }
}

GLuint ShaderOGLColor::getFramebuffer() const
{
    return framebuffer;
}