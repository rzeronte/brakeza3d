
//
// Created by edu on 17/12/23.
//

#include "../../include/OpenGL/ShaderOGLColor.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Profiler.h"

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

    // Fase 4b (picking): attributes 3-6 (mat4 aInstanceModel), mismo patrón que
    // ShaderOGLRenderDeferred -- configurado una sola vez aquí, RenderColorInstanced() solo
    // respecifica el contenido en cada batch.
    glGenBuffers(1, &instanceModelBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
            reinterpret_cast<void*>(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }

    // attribute 7 (vec3 aInstanceColor) -- color de picking por instancia.
    glGenBuffers(1, &instanceColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceColorBuffer);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(7, 1);

    // Picking instanciado con skinning (Fase 2): TBO con las matrices de huesos del batch,
    // leído en GLSL/Color.vs como samplerBuffer. glTexBuffer() solo hace falta una vez.
    glGenBuffers(1, &boneMatrixBuffer);
    glGenTextures(1, &boneMatrixTexture);
    glBindTexture(GL_TEXTURE_BUFFER, boneMatrixTexture);
    glBindBuffer(GL_TEXTURE_BUFFER, boneMatrixBuffer);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, boneMatrixBuffer);
}

void ShaderOGLColor::LoadUniforms()
{
    // projection/view ya no son uniforms sueltos: se leen de CameraBlock (binding 3),
    // relleno una vez por frame en ComponentRender::UpdateCameraUBO().
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "CameraBlock"), 3);

    // alpha (Color.fs): un uniform no fijado vale 0. Red de seguridad: 1 desde el principio; aun así
    // cada consumidor lo fija explícitamente (los uniforms persisten entre usos del programa).
    Components::get()->Render()->ChangeOpenGLProgram(programID);
    setFloat("alpha", 1.0f);
}

void ShaderOGLColor::renderMesh(Mesh3D* m, bool useFeedbackBuffer, const Color &color, bool clearFramebuffer, GLuint fbo) const
{
    for (const auto& mm : m->getMeshData()) {
        if (!mm.visibleInFrustum) continue;
        RenderColor(
            m->getModelMatrix(),
            useFeedbackBuffer ? mm.feedbackBuffer : mm.vertexBuffer,
            mm.uvBuffer,
            useFeedbackBuffer ? mm.feedbackNormalBuffer : mm.normalBuffer,
            static_cast<int>(mm.vertices.size()),
            color,
            clearFramebuffer,
            fbo,
            useFeedbackBuffer ? 0 : mm.indexBuffer,
            useFeedbackBuffer ? 0 : mm.indexCount
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
    GLuint fbo,
    GLuint indexBuffer,
    GLsizei indexCount
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

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    // projection/view: CameraBlock (binding 3), no hace falta subirlas aqui.
    setBool("useInstancing", false);
    setBool("useSkinning", false);
    setFloat("alpha", 1.0f);   // picking: opaco (ver RenderTint)
    setMat4("model", modelView);
    setVec3("color", c.toGLM());

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    DrawMeshGeometry(GL_TRIANGLES, indexBuffer, indexCount, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    render->ChangeOpenGLFramebuffer(0);
}

// Tinte de color translúcido sobre una geometría (edificio seleccionado: render:drawFillSubmesh).
// Mismo programa que el picking, pero mezclado con alpha sobre fbo, sin prueba ni escritura de
// profundidad (como el contorno: se ve aunque algo lo tape) y solo caras frontales (menos doble
// mezcla donde se solapan caras del mismo mesh). Deja el estado como RenderColor al salir.
void ShaderOGLColor::RenderTint(
    const glm::mat4 &model,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int size,
    const Color &c,
    float alpha,
    GLuint fbo,
    GLuint indexBuffer,
    GLsizei indexCount
) const
{
    auto render = Components::get()->Render();
    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);
    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidthRender(), window->getHeightRender());

    glBindVertexArray(VertexArrayID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    setBool("useInstancing", false);
    setBool("useSkinning", false);
    setMat4("model", model);
    setVec3("color", c.toGLM());
    setFloat("alpha", alpha);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    DrawMeshGeometry(GL_TRIANGLES, indexBuffer, indexCount, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    setFloat("alpha", 1.0f);   // no dejar el programa (compartido con el picking) translúcido
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    render->ChangeOpenGLFramebuffer(0);
}

// Fase 4b: dibuja un run de 2+ submeshes de picking que comparten geometria (ver
// ComponentRender::FlushPickingQueue) con UNA sola llamada instanciada. A diferencia del opaco,
// el color NO se comparte -- es precisamente el dato por-instancia que distingue que objeto
// concreto fue pulsado, así que viaja en instanceColors paralelo a instanceModels.
void ShaderOGLColor::RenderColorInstanced(
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int size,
    GLuint fbo,
    GLuint indexBuffer,
    GLsizei indexCount,
    const std::vector<glm::mat4> &instanceModels,
    const std::vector<glm::vec3> &instanceColors
) const
{
    if (instanceModels.empty()) return;

    auto render = Components::get()->Render();
    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);
    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidthRender(), window->getHeightRender());

    glBindVertexArray(VertexArrayID);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    setBool("useInstancing", true);
    setBool("useSkinning", false);
    setFloat("alpha", 1.0f);   // picking: opaco (ver RenderTint)

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(instanceModels.size() * sizeof(glm::mat4)),
        instanceModels.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, instanceColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(instanceColors.size() * sizeof(glm::vec3)),
        instanceColors.data(), GL_DYNAMIC_DRAW);

    DrawMeshGeometryInstanced(GL_TRIANGLES, indexBuffer, indexCount, size, static_cast<GLsizei>(instanceModels.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    render->ChangeOpenGLFramebuffer(0);
}

// Fase 2: picking instanciado con skinning -- mismo cuerpo que RenderColorInstanced(), salvo que
// la geometria viene de vertexBuffer/vertexBoneDataBuffer COMPARTIDOS (bind-pose) y el skinning se
// aplica en GLSL/Color.vs leyendo boneMatrixBuffer/Texture (TBO) indexado por gl_InstanceID. Se usa
// siempre para entradas skinned, incluso un batch de tamaño 1 (ver ComponentRender::FlushPickingQueue).
void ShaderOGLColor::RenderColorInstancedSkinned(
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    GLuint vertexBoneDataBuffer,
    int size,
    GLuint fbo,
    int bonesPerInstance,
    const std::vector<glm::mat4> &instanceModels,
    const std::vector<glm::vec3> &instanceColors,
    const std::vector<glm::mat4> &allBoneMatrices
) const
{
    if (instanceModels.empty() || bonesPerInstance <= 0) return;

    auto render = Components::get()->Render();
    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);
    auto window = Components::get()->Window();
    glViewport(0,0, window->getWidthRender(), window->getHeightRender());

    glBindVertexArray(VertexArrayID);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    setBool("useInstancing", true);
    setBool("useSkinning", true);
    setFloat("alpha", 1.0f);   // picking: opaco (ver RenderTint)
    setInt("bonesPerInstance", bonesPerInstance);
    setInt("boneMatrices", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, boneMatrixTexture);

    setVAOAttributes(vertexBuffer, uvBuffer, normalBuffer);
    setVAOBoneAttributes(vertexBoneDataBuffer, 8, 9);

    glBindBuffer(GL_ARRAY_BUFFER, instanceModelBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(instanceModels.size() * sizeof(glm::mat4)),
        instanceModels.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, instanceColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(instanceColors.size() * sizeof(glm::vec3)),
        instanceColors.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_TEXTURE_BUFFER, boneMatrixBuffer);
    glBufferData(GL_TEXTURE_BUFFER, static_cast<GLsizeiptr>(allBoneMatrices.size() * sizeof(glm::mat4)),
        allBoneMatrices.data(), GL_DYNAMIC_DRAW);

    // indexBuffer=0: geometria animada nunca paso por el EBO deduplicado -- siempre glDrawArraysInstanced.
    DrawMeshGeometryInstanced(GL_TRIANGLES, 0, 0, size, static_cast<GLsizei>(instanceModels.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(8);
    glDisableVertexAttribArray(9);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(framebuffer);

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
    if (depthBuffer != 0) {
        glDeleteRenderbuffers(1, &depthBuffer);
        depthBuffer = 0;
    }
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
}

GLuint ShaderOGLColor::getFramebuffer() const
{
    return framebuffer;
}