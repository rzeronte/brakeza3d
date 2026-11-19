/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_SHADEROGL_GROUNDCIRCLES_H
#define BRAKEZA3D_SHADEROGL_GROUNDCIRCLES_H

#include "Base/ShaderBaseOpenGL.h"

// Versión por instancias de ShaderOGLGroundCircle: todos los círculos (o discos "blob") de una tanda en un solo glDrawArraysInstanced.
class ShaderOGLGroundCircles : public ShaderBaseOpenGL
{
public:
    // Floats por círculo: x, y, z, radio, r, g, b, a, grosor
    static constexpr int STRIDE = 9;

private:
    GLuint vao         = 0;
    GLuint quadVbo     = 0;
    GLuint instanceVbo = 0;
    size_t instanceCapacity = 0;   // círculos que caben en instanceVbo

    GLuint viewUniform       = 0;
    GLuint projectionUniform = 0;
    GLuint gPositionUniform  = 0;
    GLuint screenSizeUniform = 0;
    GLuint filledUniform     = 0;

public:
    ShaderOGLGroundCircles();

    void PrepareMainThread() override;
    void LoadUniforms() override;
    void Destroy() override;

    // data: count * STRIDE floats
    void draw(const float* data, size_t count, GLuint fbo, bool filled);
};

#endif
