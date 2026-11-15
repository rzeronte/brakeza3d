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
// Created by Eduardo on 19/03/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H
#define BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H


#include "Base/ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"

class ShaderOGLBonesTransforms : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;
    GLuint gBonesUniform = 0;
public:
    ShaderOGLBonesTransforms();

    void PrepareBackground() override;
    void PrepareMainThread() override;

    void LoadUniforms() override;
    void render(Mesh3DData &meshData, const std::vector<glm::mat4>& transformations, GLuint fbo);
    void setVAOAttributes(GLuint vertexbuffer, GLuint vertexBoneDataBuffer, GLuint normalBuffer);

    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H
