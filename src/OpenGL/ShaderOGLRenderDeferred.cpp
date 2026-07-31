#define GL_GLEXT_PROTOTYPES

#include "../../include/OpenGL/ShaderOGLRenderDeferred.h"
#include "../../include/Components/Components.h"

ShaderOGLRenderDeferred::ShaderOGLRenderDeferred()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "GBuffer.vs",
        Config::get()->SHADERS_FOLDER + "GBuffer.fs",
        false
    )
{
}

void ShaderOGLRenderDeferred::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    LoadUniforms();
}

void ShaderOGLRenderDeferred::LoadUniforms()
{
    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");
    drawOffsetUniform = glGetUniformLocation(programID, "drawOffset");
    textureDiffuseUniform = glGetUniformLocation(programID, "texture_diffuse");
    textureSpecularUniform = glGetUniformLocation(programID, "texture_specular");
    alphaUniform = glGetUniformLocation(programID, "alpha");
}

void ShaderOGLRenderDeferred::renderMesh(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo) const
{
    const auto& textures  = o->getModelTextures();
    const auto& specTextures = o->getModelSpecularTextures();
    for (const auto& m: o->getMeshData()) {
        if (!m.visibleInFrustum) continue;
        if (m.materialIndex < 0 || (size_t)m.materialIndex >= textures.size() ||
            (size_t)m.materialIndex >= specTextures.size()) continue;
        auto* tex = textures[m.materialIndex];
        auto* specTex = specTextures[m.materialIndex];
        if (!tex || !specTex) continue;
        render(
            o,
            tex->getOGLTextureID(),
            specTex->getOGLTextureID(),
            useFeedbackBuffer ? m.feedbackBuffer : m.vertexBuffer,
            m.uvBuffer,
            useFeedbackBuffer ? m.feedbackNormalBuffer : m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            o->getAlpha(),
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
    float alpha,
    GLuint fbo
) const
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidthRender(), window->getHeightRender());

    glBindVertexArray(VertexArrayID);

    auto camera = Components::get()->Camera();

    setMat4Uniform(matrixProjectionUniform, camera->getGLMMat4ProjectionMatrix());
    setMat4Uniform(matrixViewUniform, camera->getGLMMat4ViewMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());
    setVec3Uniform(drawOffsetUniform, o->getDrawOffset().toGLM());

    setFloatUniform(alphaUniform, alpha);
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

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
}

void ShaderOGLRenderDeferred::Destroy()
{
    // VAO is size-independent — nothing to destroy on resize
}
