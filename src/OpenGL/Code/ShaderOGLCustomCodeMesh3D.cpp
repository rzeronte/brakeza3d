//
// Created by Eduardo on 12/02/2025.
//

#include "../../../include/OpenGL/Code/ShaderOGLCustomCodeMesh3D.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Brakeza.h"
#include "../../../include/OpenGL/Code/ShaderCustomOGLCodeTypes.h"
#include "../../../include/Render/Profiler.h"

ShaderOGLCustomCodeMesh3D::ShaderOGLCustomCodeMesh3D(Mesh3D* mesh, const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile)
:
    ShaderBaseCustomOGLCode(label, typesFile, vsFile, fsFile, SHADER_OBJECT),
    mesh(mesh)
{
}

ShaderOGLCustomCodeMesh3D::ShaderOGLCustomCodeMesh3D(Mesh3D* mesh, const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile, cJSON* types)
:
    ShaderBaseCustomOGLCode(label, typesFile, vsFile, fsFile, SHADER_OBJECT, types),
    mesh(mesh)
{
}

void ShaderOGLCustomCodeMesh3D::LoadUniforms()
{
    LOG_MESSAGE("[ShaderOGLCustomCodeMesh3D] LoadUniforms START, programID=%d", programID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");
    alphaUniform = glGetUniformLocation(programID, "alpha");

    LOG_MESSAGE("[ShaderOGLCustomCodeMesh3D] Uniforms: proj=%d, view=%d, model=%d, alpha=%d", matrixProjectionUniform, matrixViewUniform, matrixModelUniform, alphaUniform);

    if (programID == 0) {
        LOG_ERROR("[ShaderOGLCustomCodeMesh3D] ERROR: programID is 0!");
    }
}

void ShaderOGLCustomCodeMesh3D::PrepareMainThread()
{
    ShaderBaseCustomOGLCode::PrepareMainThread();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    LoadUniforms();
}

void ShaderOGLCustomCodeMesh3D::Render(GLuint fbo, GLuint texture)
{
    if (!isEnabled()) return;
    setInternalTexture(texture);
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
            fbo,
            m.indexBuffer,
            m.indexCount
        );
    }
}

void ShaderOGLCustomCodeMesh3D::renderMesh(
    Object3D *o,
    GLuint textureID,
    GLuint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    GLuint feedbackBuffer,
    int size,
    float alpha,
    GLuint fbo,
    GLuint indexBuffer,
    GLsizei indexCount
)
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    // Este shader escribe G-Buffer (gPosition/gNormal/gAlbedo) via MRT -- necesita el mismo
    // estado opaco que ShaderOGLRenderDeferred::render(), fijado aqui de forma incondicional en
    // vez de heredar lo que dejara el draw anterior del frame. Antes "funcionaba por accidente"
    // porque el pase de picking (MOUSE_CLICK_SELECT_OBJECT3D) se dibujaba justo antes, por objeto,
    // y dejaba exactamente este estado -- al diferir picking a un flush por lotes al final del
    // frame (Fase 4b) ese acoplamiento implicito desaparecio, y objetos con shader de objeto tipo
    // WaterRTS heredaban blend/depth de lo ultimo que hubiera corrido antes (p.ej. post-processing
    // del frame anterior) en vez de un estado opaco correcto.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

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

    const bool hasFeedback = (feedbackBuffer != 0);

    if (hasFeedback) {
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbackBuffer);
        glBeginTransformFeedback(GL_TRIANGLES);
    }

    DrawMeshGeometry(GL_TRIANGLES, hasFeedback ? 0 : indexBuffer, indexCount, size);

    if (hasFeedback) {
        glEndTransformFeedback();
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    if (hasFeedback) {
        glBindBuffer(GL_COPY_READ_BUFFER, feedbackBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(glm::vec4) * size);
    }

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLCustomCodeMesh3D::setShaderSystemUniforms(GLuint diffuse, GLuint specular)
{
    for (auto type: dataTypes) {
        switch (GetGLSLTypeInfo(type.type).type) {
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
