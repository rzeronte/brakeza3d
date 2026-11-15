/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_SHADERBASECOMPUTE_H
#define BRAKEZA3D_SHADERBASECOMPUTE_H

#include "ShaderBaseOpenGL.h"
#include "../../Misc/FilePaths.h"

class ShaderBaseCompute : public ShaderBaseOpenGL {
    FilePath::ComputeShaderFile computeFilePath;
    std::string sourceCS;
public:
    explicit ShaderBaseCompute(const FilePath::ComputeShaderFile& computeFile);

    void PrepareBackground() override;
    void PrepareMainThread() override;
    void LoadUniforms() override {}
    void Destroy() override;

    void dispatch(GLuint groupsX, GLuint groupsY = 1, GLuint groupsZ = 1) const;
};

#endif //BRAKEZA3D_SHADERBASECOMPUTE_H
