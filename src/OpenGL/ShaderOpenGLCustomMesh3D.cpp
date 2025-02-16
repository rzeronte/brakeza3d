//
// Created by Eduardo on 12/02/2025.
//

#include "../include/OpenGL/ShaderOpenGLCustomMesh3D.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ShaderOpenGLCustomMesh3D::ShaderOpenGLCustomMesh3D(
    Mesh3D* mesh,
    const std::string &label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename
)
:
    mesh(mesh),
    ShaderOpenGLCustom(label, vertexFilename, fragmentFilename, ShaderCustomTypes::SHADER_OBJECT)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    alphaUniform = glGetUniformLocation(programID, "alpha");
}
ShaderOpenGLCustomMesh3D::ShaderOpenGLCustomMesh3D(
    Mesh3D* mesh,
    const std::string &label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename,
    cJSON* types
)
:
    mesh(mesh),
    ShaderOpenGLCustom(label, vertexFilename, fragmentFilename, ShaderCustomTypes::SHADER_OBJECT, types)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    alphaUniform = glGetUniformLocation(programID, "alpha");
}

void ShaderOpenGLCustomMesh3D::render(GLuint framebuffer)
{
    if (!isEnabled()) return;

    for (const auto& m: mesh->meshes) {
        renderMesh(
            mesh,
            mesh->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            mesh->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            m.vertexbuffer,
            m.uvbuffer,
            m.normalbuffer,
            (int) m.vertices.size(),
            mesh->getAlpha(),
            framebuffer
        );
    }
}

void ShaderOpenGLCustomMesh3D::renderMesh(
    Object3D *o,
    GLint textureID,
    GLint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    float alpha,
    GLuint framebuffer
)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    auto camera = ComponentsManager::get()->getComponentCamera();
    auto cameraPosition = camera->getCamera()->getPosition().toGLM();

    setMat4Uniform(matrixProjectionUniform, camera->getGLMMat4ProjectionMatrix());
    setMat4Uniform(matrixViewUniform, camera->getGLMMat4ViewMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());
    setFloatUniform(alphaUniform, alpha);

    resetNumberTextures();

    setDataTypesUniforms();
    setShaderSystemUniforms(textureID, textureSpecularID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    glDrawArrays(GL_TRIANGLES, 0, size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(0);
}

void ShaderOpenGLCustomMesh3D::setShaderSystemUniforms(GLuint diffuse, GLuint specular)
{
    for (auto type: dataTypes) {
        switch (GLSLTypeMapping[type.type]) {
            case ShaderOpenGLCustomDataType::DIFFUSE: {
                setTexture(type.name, diffuse, numTextures);
                increaseNumberTextures();
                break;
            }
            case ShaderOpenGLCustomDataType::SPECULAR: {
                setTexture(type.name, specular, numTextures);
                increaseNumberTextures();
                break;
            }
            default:
                break;
        }
    }
}

void ShaderOpenGLCustomMesh3D::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}