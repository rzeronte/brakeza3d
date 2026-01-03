//
// Created by Eduardo on 12/02/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H
#define BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H


#include "ShaderCustomOGLCode.h"
#include "../../3D/Mesh3D.h"

class ShaderOGLCustomCodeMesh3D : public ShaderCustomOGLCode
{
    GLuint VertexArrayID = 0;

    GLuint matrixProjectionUniform = 0;
    GLuint matrixViewUniform = 0;
    GLuint matrixModelUniform = 0;
    GLuint alphaUniform = 0;

    Mesh3D* mesh = nullptr;
public:
    ShaderOGLCustomCodeMesh3D(Mesh3D* mesh, const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile);
    ShaderOGLCustomCodeMesh3D(Mesh3D* mesh, const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile, cJSON* types );

    void LoadUniforms() override;
    void PrepareMainThread() override;
    void render(GLuint fbo, GLuint texture) override;

    void renderMesh(
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
    );
    void setShaderSystemUniforms(GLuint diffuse, GLuint specular);
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H
