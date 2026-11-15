/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_SHADEROGL_CIRCLE2D_H
#define BRAKEZA3D_SHADEROGL_CIRCLE2D_H

#include "Base/ShaderBaseOpenGL.h"
#include "../Render/Color.h"

class ShaderOGLCircle2D : public ShaderBaseOpenGL
{
    GLuint quadVAO = 0;
    GLuint VBO     = 0;

    GLuint modelMatrixUniform      = 0;
    GLuint projectionMatrixUniform = 0;
    GLuint waveColorUniform        = 0;
    GLuint timeUniform             = 0;
    GLuint numWavesUniform         = 0;
    GLuint speedUniform            = 0;
    GLuint thicknessUniform        = 0;

public:
    ShaderOGLCircle2D();

    void CreateQuadVBO();
    void PrepareMainThread() override;
    void LoadUniforms() override;

    void renderCircle2D(
        int x, int y, int w, int h,
        int worldW, int worldH,
        const Color &c,
        float numWaves, float speed, float thickness,
        bool additive,
        GLuint fbo
    ) const;

    void Destroy() override;
};

#endif
