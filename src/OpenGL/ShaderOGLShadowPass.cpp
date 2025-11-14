//
// Created by Eduardo on 13/11/2025.
//

#include "../../include/OpenGL/ShaderOGLShadowPass.h"
#include "../../include/EngineSetup.h"
#include "../../include/ComponentsManager.h"

ShaderOGLShadowPass::ShaderOGLShadowPass()
:
    VertexArrayID(0),
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "ShadowPass.vs",
        EngineSetup::get()->SHADERS_FOLDER + "ShadowPass.fs",
        false
    )
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixViewUniform = glGetUniformLocation(programID, "lightSpaceMatrix");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    setupFBO();
}

void ShaderOGLShadowPass::render(
    Object3D* o,
    LightPoint3D* light,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    GLuint shadowMapArrayTex,
    int layer,
    GLuint framebuffer
) const
{
    if (light == nullptr) {
        Logging::Message("ShaderShadowPass Error: Empty LightPoint3D!!");
        return;
    }

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    setMat4Uniform(matrixViewUniform, light->getLightSpaceMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapArrayTex, 0, layer);

    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(0);
}

GLuint ShaderOGLShadowPass::getShadowFBO() const {
    return shadowFBO;
}

void ShaderOGLShadowPass::renderMesh(Mesh3D *o, LightPoint3D* light, GLuint shadowMapArrayTex, int lightIndex, GLuint framebuffer) {
    for (const auto& m: o->meshes) {
        render(
            o,
            light,
            m.vertexbuffer,
            m.uvbuffer,
            m.normalbuffer,
            static_cast<int>(m.vertices.size()),
            shadowMapArrayTex,
            lightIndex,
            framebuffer
        );
    }
}

void ShaderOGLShadowPass::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
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

void ShaderOGLShadowPass::destroy()
{
}

void ShaderOGLShadowPass::setupFBO()
{
    if (shadowFBO != 0) {
        glDeleteFramebuffers(1, &shadowFBO);
    }

    auto shadowMapArrayTex = ComponentsManager::get()->getComponentWindow()->getShadowMapArrayTex();

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapArrayTex, 0, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}