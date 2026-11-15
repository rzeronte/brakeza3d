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
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLTINT_H
#define BRAKEZA3D_SHADEROPENGLTINT_H


#include "../Base/ShaderBaseOpenGL.h"
#include "../Base/ShaderBaseOpenGLQuad.h"
#include "../../Render/Color.h"

class ShaderOGLTint : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    GLuint colorUniform = 0;
    GLuint alphaUniform = 0;

public:
    ShaderOGLTint();
    void LoadUniforms() override;

    void PrepareMainThread() override;

    void render(Color c, float alpha, GLuint fbo);

    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLTINT_H
