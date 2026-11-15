/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_SHADEROPENGLRECT_H
#define BRAKEZA3D_SHADEROPENGLRECT_H

#include "Base/ShaderBaseOpenGL.h"
#include "../Render/Color.h"

class ShaderOGLRect : public ShaderBaseOpenGL
{
    GLuint quadVAO = 0;
    GLuint VBO = 0;

    GLuint modelMatrixUniform      = 0;
    GLuint projectionMatrixUniform = 0;
    GLuint rectColorUniform        = 0;

public:
    ShaderOGLRect();

    void CreateQuadVBO();
    void PrepareMainThread() override;
    void LoadUniforms() override;

    void renderRect(int x, int y, int w, int h, int worldW, int worldH, const Color &c, GLuint fbo) const;

    void Destroy() override;
};

#endif //BRAKEZA3D_SHADEROPENGLRECT_H
