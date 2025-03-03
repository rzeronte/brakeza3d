//
// Created by Eduardo on 12/02/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H
#define BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H


#include "ShaderOpenGLCustom.h"
#include "../Objects/Mesh3D.h"

class ShaderOpenGLCustomMesh3D : public ShaderOpenGLCustom {
    GLuint VertexArrayID;

    GLint matrixProjectionUniform;
    GLint matrixViewUniform;
    GLint matrixModelUniform;
    GLint alphaUniform;

    Mesh3D* mesh;
public:
    ShaderOpenGLCustomMesh3D(
        Mesh3D* mesh,
        const std::string &label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename
    );

    ShaderOpenGLCustomMesh3D(
        Mesh3D* mesh,
        const std::string &label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename,
        cJSON* types
    );

    void render(GLuint framebuffer);

    void renderMesh(
        Object3D *o,
        GLint textureID,
        GLint textureSpecularID,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        GLuint feedbackBuffer,
        int size,
        float alpha,
        GLuint framebuffer
    );

    void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer);

    void setShaderSystemUniforms(GLuint diffuse, GLuint specular);
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H
