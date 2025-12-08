//
// Created by Eduardo on 12/02/2025.
//

#include "../include/OpenGL/ShaderOGLCustomMesh3D.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Brakeza.h"

ShaderOGLCustomMesh3D::ShaderOGLCustomMesh3D(
    Mesh3D* mesh,
    const std::string &label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename
)
:
    ShaderOGLCustom(label, vertexFilename, fragmentFilename, ShaderCustomType::SHADER_OBJECT),
    mesh(mesh)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    alphaUniform = glGetUniformLocation(programID, "alpha");
}

ShaderOGLCustomMesh3D::ShaderOGLCustomMesh3D(
    Mesh3D* mesh,
    const std::string &label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename,
    cJSON* types
)
:
    ShaderOGLCustom(label, vertexFilename, fragmentFilename, ShaderCustomType::SHADER_OBJECT, types),
    VertexArrayID(0),
    mesh(mesh)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    alphaUniform = glGetUniformLocation(programID, "alpha");
}

void ShaderOGLCustomMesh3D::render(GLuint fbo)
{
    if (!isEnabled()) return;

    for (const auto& m: mesh->getMeshData()) {
        renderMesh(
            mesh,
            mesh->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            mesh->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            m.vertexBuffer,
            m.uvBuffer,
            m.normalBuffer,
            m.feedbackBuffer,
            m.vertices.size(),
            mesh->getAlpha(),
            fbo
        );
    }
}

void ShaderOGLCustomMesh3D::renderMesh(
    Object3D *o,
    GLuint textureID,
    GLuint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    GLuint feedbackBuffer,
    int size,
    float alpha,
    GLuint fbo
)
{
    ComponentsManager::get()->Render()->changeOpenGLFramebuffer(fbo);
    ComponentsManager::get()->Render()->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    auto camera = ComponentsManager::get()->Camera();

    setMat4Uniform(matrixProjectionUniform, camera->getGLMMat4ProjectionMatrix());
    setMat4Uniform(matrixViewUniform, camera->getGLMMat4ViewMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());
    setFloatUniform(alphaUniform, alpha);

    ResetNumberTextures();

    setDataTypesUniforms();
    setShaderSystemUniforms(textureID, textureSpecularID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbackBuffer);  // Vinculamos el buffer de feedback
    glBeginTransformFeedback(GL_TRIANGLES);  // Especificamos el tipo de primitivas que estamos procesando

    glDrawArrays(GL_TRIANGLES, 0, size );

    glEndTransformFeedback();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // Copia los datos del feedbackBuffer al vertexbuffer
    glBindBuffer(GL_COPY_READ_BUFFER, feedbackBuffer);  // Vincula el buffer de feedback como buffer de lectura
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);      // Vincula el buffer de vértices como buffer de escritura
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(glm::vec4) * size);

    ComponentsManager::get()->Render()->changeOpenGLFramebuffer(0);
}

void ShaderOGLCustomMesh3D::setShaderSystemUniforms(GLuint diffuse, GLuint specular)
{
    for (auto type: dataTypes) {
        switch (GLSLTypeMapping[type.type].type) {
            case ShaderOpenGLCustomDataType::DIFFUSE: {
                setTexture(type.name, diffuse, numTextures);
                IncreaseNumberTextures();
                break;
            }
            case ShaderOpenGLCustomDataType::SPECULAR: {
                setTexture(type.name, specular, numTextures);
                IncreaseNumberTextures();
                break;
            }
            default:
                break;
        }
    }
}
