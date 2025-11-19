
//
// Created by edu on 17/12/23.
//

#include "../../include/OpenGL/ShaderOpenGLColor.h"
#include "../../include/ComponentsManager.h"


ShaderOpenGLColor::ShaderOpenGLColor()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Color.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Color.fs",
        false
    ),
    VertexArrayID(0),
    framebuffer(0),
    textureColorbuffer(0)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    createBuffer();
}

void ShaderOpenGLColor::renderMesh(Mesh3D* m, const Color &color, bool clearFramebuffer, GLuint framebuffer) const
{
    for (const auto& mm : m->meshes) {
        renderColor(
            m->getModelMatrix(),
            mm.vertexbuffer,
            mm.uvbuffer,
            mm.normalbuffer,
            static_cast<int>(mm.vertices.size()),
            color,
            clearFramebuffer,
            framebuffer
        );
    }
}

void ShaderOpenGLColor::renderColor(
    const glm::mat4 &modelView,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    const Color &color,
    bool clearFramebufer,
    GLuint framebuffer
) const
{
    auto render = ComponentsManager::get()->getComponentRender();

    render->changeOpenGLFramebuffer(framebuffer);
    render->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    if (clearFramebufer) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    auto camera = ComponentsManager::get()->getComponentCamera();

    setMat4("projection", camera->getGLMMat4ProjectionMatrix());
    setMat4("view", camera->getGLMMat4ViewMatrix());
    setMat4("model", modelView);
    setVec3("color", color.toGLM());

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    glDrawArrays(GL_TRIANGLES, 0,  size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    render->changeOpenGLFramebuffer(0);
}

void ShaderOpenGLColor::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,                  // attribute
            4,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            nullptr
    );
    // 3rd attribute buffer: normals
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            1,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr
    );
    // 2nd attribute buffer: UVs
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
            2,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr
    );
}

void ShaderOpenGLColor::destroy()
{
    createBuffer();
}

void ShaderOpenGLColor::deleteTexture()
{
    glDeleteTextures(1, &textureColorbuffer);
}

GLuint ShaderOpenGLColor::getTextureColorBuffer() const
{
    return textureColorbuffer;
}

void ShaderOpenGLColor::createBuffer()
{
    if (framebuffer != 0) {
        glDeleteFramebuffers(1, &framebuffer);
    }
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    const int w = ComponentsManager::get()->getComponentWindow()->getWidth();
    const int h = ComponentsManager::get()->getComponentWindow()->getHeight();

    if (textureColorbuffer != 0) {
        glDeleteTextures(1, &textureColorbuffer);
        textureColorbuffer = 0;
    }

    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

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

GLuint ShaderOpenGLColor::getFramebuffer() const
{
    return framebuffer;
}