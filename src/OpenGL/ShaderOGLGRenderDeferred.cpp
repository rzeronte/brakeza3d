#define GL_GLEXT_PROTOTYPES

#include "../../include/OpenGL/ShaderOGLGRenderDeferred.h"
#include "../../include/ComponentsManager.h"

ShaderOGLGRenderDeferred::ShaderOGLGRenderDeferred()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "GBuffer.vs",
        EngineSetup::get()->SHADERS_FOLDER + "GBuffer.fs",
        false
    ),
    VertexArrayID(0)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");
    drawOffsetUniform = glGetUniformLocation(programID, "drawOffset");
    textureDiffuseUniform = glGetUniformLocation(programID, "texture_diffuse");
    textureSpecularUniform = glGetUniformLocation(programID, "texture_specular");
}

void ShaderOGLGRenderDeferred::renderMesh(Mesh3D *o, GLuint framebuffer)
{
    for (const auto& m: o->meshes) {
        render(
            o,
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            o->getModelSpecularTextures()[m.materialIndex]->getOGLTextureID(),
            m.vertexbuffer,
            m.uvbuffer,
            m.normalbuffer,
            static_cast<int>(m.vertices.size()),
            framebuffer
        );
    }
}

void ShaderOGLGRenderDeferred::renderAnimatedMesh(Mesh3D *o, GLuint framebuffer)
{
    for (const auto& m: o->meshes) {
        render(
            o,
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            o->getModelSpecularTextures()[m.materialIndex]->getOGLTextureID(),
            m.feedbackBuffer,
            m.uvbuffer,
            m.normalbuffer,
            static_cast<int>(m.vertices.size()),
            framebuffer
        );
    }
}

void ShaderOGLGRenderDeferred::render(
    Object3D *o,
    GLint textureID,
    GLint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    GLuint framebuffer
)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);

    glBindVertexArray(VertexArrayID);

    auto camera = ComponentsManager::get()->getComponentCamera();

    setMat4Uniform(matrixProjectionUniform, camera->getGLMMat4ProjectionMatrix());
    setMat4Uniform(matrixViewUniform, camera->getGLMMat4ViewMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());
    setVec3Uniform(drawOffsetUniform, o->getDrawOffset().toGLM());

    setIntUniform(textureDiffuseUniform, 0);
    setIntUniform(textureSpecularUniform, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecularID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
}

void ShaderOGLGRenderDeferred::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShaderOGLGRenderDeferred::destroy()
{
    if (VertexArrayID != 0) {
        glDeleteVertexArrays(1, &VertexArrayID);
        VertexArrayID = 0;
    }
}
