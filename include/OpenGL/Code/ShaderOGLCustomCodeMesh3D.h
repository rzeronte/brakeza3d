/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by Eduardo on 12/02/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H
#define BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H


#include "ShaderBaseCustomOGLCode.h"
#include "../../3D/Mesh3D.h"

class ShaderOGLCustomCodeMesh3D : public ShaderBaseCustomOGLCode
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
    void Render(GLuint fbo, GLuint texture) override;

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
        GLuint fbo,
        GLuint indexBuffer = 0,
        GLsizei indexCount = 0
    );
    void setShaderSystemUniforms(GLuint diffuse, GLuint specular);
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOMMESH3D_H
