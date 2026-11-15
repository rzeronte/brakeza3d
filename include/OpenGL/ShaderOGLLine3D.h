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
// Created by Eduardo on 24/01/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLLINE3D_H
#define BRAKEZA3D_SHADEROPENGLLINE3D_H


#include "Base/ShaderBaseOpenGL.h"
#include "../3D/Vertex3D.h"
#include "../Render/Color.h"
#include "../3D/Vector3D.h"
#include <vector>

class ShaderOGLLine3D : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;
    GLuint vertexbuffer = 0;
    glm::vec3 vertices[2];

    GLuint matrixProjectionUniform = 0;
    GLuint matrixViewUniform = 0;
    GLuint colorUniform = 0;

    void Destroy() override;

public:
    void CreateLineVBO();

    ShaderOGLLine3D();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void render(Vertex3D from, Vertex3D to, GLuint fbo, Color c);
    void renderLines(const std::vector<Vector3D>&, GLuint fbo, Color c);
};


#endif //BRAKEZA3D_SHADEROPENGLLINE3D_H
