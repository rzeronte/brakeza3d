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
// Created by edu on 26/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLDEPTHMAP_H
#define BRAKEZA3D_SHADEROPENGLDEPTHMAP_H

#include "../Base/ShaderBaseOpenGL.h"
#include "../Base/ShaderBaseOpenGLQuad.h"

class ShaderOGLDepthMap : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    GLuint textureUniform = 0;
    GLuint intensityUniform = 0;
    GLuint nearUniform = 0;
    GLuint farUniform = 0;

public:
    ShaderOGLDepthMap();

    void LoadUniforms() override;
    void PrepareMainThread() override;
    void Render(GLuint textureID, GLuint fbo);
    void Destroy() override;

    float intensity;
    float farPlane;
    float nearPlane;
};


#endif //BRAKEZA3D_SHADEROPENGLDEPTHMAP_H
