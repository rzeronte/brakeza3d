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

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
#define BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H


#include "ShaderBaseCustomOGLCode.h"

class ShaderOGLCustomCodePostprocessing : public ShaderBaseCustomOGLCode {
public:
    ShaderOGLCustomCodePostprocessing(const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile);
    ShaderOGLCustomCodePostprocessing(const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile, cJSON *types);
    void LoadUniforms() override;
    void PrepareMainThread() override;
    void Render(GLuint fbo, GLuint texture) override;
    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
