//
// Created by Eduardo on 04/01/2026.
//

#include "../../../include/OpenGL/Nodes/ShaderNodesMesh3D.h"
#include "../../../include/GUI/ShaderNodeEditorManager.h"
#include "../../../include/Components/Components.h"

ShaderNodesMesh3D::ShaderNodesMesh3D(
    const std::string& label,
    const std::string& typesFile,
    ShaderCustomType type,
    ShaderNodeEditorManager* nodeManager,
    Mesh3D* mesh
)
:
    ShaderBaseNodes(label, typesFile, type, nodeManager),
    nodeManager(nodeManager),
    currentMesh(mesh)
{
}

void ShaderNodesMesh3D::LoadUniforms()
{
    // Los uniforms se cargan automáticamente por el shader generado
}

void ShaderNodesMesh3D::Destroy()
{
}

void ShaderNodesMesh3D::Reload()
{
}

void ShaderNodesMesh3D::RenderMesh(
    Mesh3D* mesh,
    int materialIndex,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int vertexCount,
    GLuint fbo
)
{
    LOG_MESSAGE("ENTRO");
    if (!isEnabled()) return;

    currentMesh = mesh;

    // Actualizar texturas del material actual
    GLuint diffuseTexture = mesh->getModelTextures()[materialIndex]->getOGLTextureID();
    GLuint specularTexture = diffuseTexture; // Por ahora usar la misma

    nodeManager->UpdateMeshTextures(diffuseTexture, specularTexture);
    nodeManager->Update();

    // Configurar framebuffer y shader
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(nodeManager->GetShaderProgram());

    auto camera = Components::get()->Camera();

    // Enviar uniforms MVP
    GLuint shaderProgram = nodeManager->GetShaderProgram();
    GLint projLoc = glGetUniformLocation(shaderProgram, "u_Projection");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "u_View");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "u_Model");

    if (projLoc != -1) {
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &camera->getGLMMat4ProjectionMatrix()[0][0]);
    }
    if (viewLoc != -1) {
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &camera->getGLMMat4ViewMatrix()[0][0]);
    }
    if (modelLoc != -1) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &mesh->getModelMatrix()[0][0]);
    }

    // Configurar texturas (llamado por RenderEffect internamente)
    nodeManager->RenderEffect(fbo);

    // Bindear atributos de vértices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Renderizar
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderNodesMesh3D::DrawImGuiProperties(const Image* diffuse, Image* specular)
{
    //ShaderBaseNodes::DrawImGuiProperties(diffuse, specular);
}