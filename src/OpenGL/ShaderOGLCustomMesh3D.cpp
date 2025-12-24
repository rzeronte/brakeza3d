//
// Created by Eduardo on 12/02/2025.
//

#include "../include/OpenGL/ShaderOGLCustomMesh3D.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"

ShaderOGLCustomMesh3D::ShaderOGLCustomMesh3D(Mesh3D* mesh, const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile)
:
    ShaderOGLCustom(label, typesFile, vsFile, fsFile, SHADER_OBJECT),
    mesh(mesh)
{
}

ShaderOGLCustomMesh3D::ShaderOGLCustomMesh3D(Mesh3D* mesh, const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile, cJSON* types)
:
    ShaderOGLCustom(label, typesFile, vsFile, fsFile, SHADER_OBJECT, types),
    mesh(mesh)
{
}

void ShaderOGLCustomMesh3D::LoadUniforms()
{
    LOG_MESSAGE("[ShaderOGLCustomMesh3D] LoadUniforms START, programID=%d", programID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");
    alphaUniform = glGetUniformLocation(programID, "alpha");

    LOG_MESSAGE("[ShaderOGLCustomMesh3D] Uniforms: proj=%d, view=%d, model=%d, alpha=%d",
                    matrixProjectionUniform, matrixViewUniform, matrixModelUniform, alphaUniform);

    if (programID == 0) {
        LOG_ERROR("[ShaderOGLCustomMesh3D] ERROR: programID is 0!");
    }
}

void ShaderOGLCustomMesh3D::PrepareMainThread()
{
    ShaderOGLCustom::PrepareMainThread();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    LoadUniforms();
}

void ShaderOGLCustomMesh3D::render(GLuint fbo, GLuint texture)
{
    if (!isEnabled()) return;
    setTextureResult(texture);
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
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    auto camera = Components::get()->Camera();

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

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
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
