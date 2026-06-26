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
