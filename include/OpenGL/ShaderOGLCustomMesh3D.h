//
// Created by Eduardo on 12/02/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H
#define BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H


#include "ShaderOGLCustom.h"
#include "../3D/Mesh3D.h"

class ShaderOGLCustomMesh3D : public ShaderOGLCustom {
    GLuint VertexArrayID;

    GLuint matrixProjectionUniform;
    GLuint matrixViewUniform;
    GLuint matrixModelUniform;
    GLuint alphaUniform;

    Mesh3D* mesh;
public:
    ShaderOGLCustomMesh3D(
        Mesh3D* mesh,
        const std::string &label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename
    );

    ShaderOGLCustomMesh3D(
        Mesh3D* mesh,
        const std::string &label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename,
        cJSON* types
    );

    void render(GLuint fbo) override;

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
