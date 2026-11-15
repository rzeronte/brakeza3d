/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_FBXLIGHTLOADER_H
#define BRAKEZA3D_FBXLIGHTLOADER_H

#include <vector>
#include "../3D/Object3D.h"
#include "../Misc/FilePaths.h"
#include <glm/glm.hpp>

class FBXLightLoader {
public:
    // posX/Y/Z: world offset  rotX/Y/Z: euler degrees  scale: uniform scale
    static std::vector<Object3D*> LoadLightsFromFile(
        const FilePath::ModelFile &fileName,
        float posX = 0.f, float posY = 0.f, float posZ = 0.f,
        float rotX = 0.f, float rotY = 0.f, float rotZ = 0.f,
        float scale = 1.f,
        bool enabledByDefault = true
    );
};

#endif
