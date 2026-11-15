/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_SHADEROGL_GROUNDDECAL_H
#define BRAKEZA3D_SHADEROGL_GROUNDDECAL_H

#include "Base/ShaderBaseOpenGL.h"
#include "../Render/Color.h"
#include <map>
#include <string>

class Object3D;

class ShaderOGLGroundDecal : public ShaderBaseOpenGL
{
    GLuint vao = 0;

    GLuint viewUniform       = 0;
    GLuint projectionUniform = 0;
    GLuint tintUniform       = 0;
    GLuint textureUniform    = 0;

    std::map<std::string, GLuint> textureCache;

    GLuint loadTexture(const std::string& path);

public:
    ShaderOGLGroundDecal();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void Destroy() override;

    void draw(Object3D* obj, const std::string& texturePath, const Color& tint, float radius, GLuint fbo);
};

#endif //BRAKEZA3D_SHADEROGL_GROUNDDECAL_H
