#define GL_GLEXT_PROTOTYPES

#include "../../include/OpenGL/ShaderOGLRenderDeferred.h"
#include "../../include/ComponentsManager.h"

ShaderOGLRenderDeferred::ShaderOGLRenderDeferred()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "GBuffer.vs",
        EngineSetup::get()->SHADERS_FOLDER + "GBuffer.fs",
        false
    )
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

void ShaderOGLRenderDeferred::renderMesh(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo)
{
    for (const auto& m: o->getMeshData()) {
        render(
            o,
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            o->getModelSpecularTextures()[m.materialIndex]->getOGLTextureID(),
            useFeedbackBuffer ? m.feedbackBuffer : m.vertexBuffer,
            m.uvBuffer,
            m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            fbo
        );
    }
}

void ShaderOGLRenderDeferred::render(
    Object3D *o,
    GLuint texId,
    GLuint specTexId,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int size,
    GLuint fbo
) const
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(fbo);
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
    glBindTexture(GL_TEXTURE_2D, texId);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specTexId);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);

    glEnable(GL_BLEND);
}

void ShaderOGLRenderDeferred::destroy()
{
    if (VertexArrayID != 0) {
        glDeleteVertexArrays(1, &VertexArrayID);
        VertexArrayID = 0;
    }
}
