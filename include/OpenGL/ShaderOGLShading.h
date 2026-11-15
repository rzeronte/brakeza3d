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
// Created by edu on 12/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLSHADING_H
#define BRAKEZA3D_SHADEROPENGLSHADING_H


#include "Base/ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"

class ShaderOGLShading : public ShaderBaseOpenGL {
    GLuint VertexArrayID = 0;
public:
    ShaderOGLShading();
    void LoadUniforms() override;

    void renderMesh(Mesh3D *mesh, bool useFeedbackBuffer, GLuint fbo);
    void render(glm::mat4 modelMatrix, GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer, int size, GLuint fbo, GLuint indexBuffer = 0, GLsizei indexCount = 0) const;
    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLSHADING_H
