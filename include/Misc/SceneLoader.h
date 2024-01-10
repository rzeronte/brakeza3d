//
// Created by eduardo on 10/10/23.
//

#ifndef BRAKEZA3D_SCENELOADER_H
#define BRAKEZA3D_SCENELOADER_H

#include <string>
#include <map>
#include "../Objects/Vertex3D.h"
#include "cJSON.h"
#include "Color.h"

enum class SceneObjectLoaderMapping {
    Object3D,
    Mesh3D,
    LightPoint3D,
    Mesh3DGhost,
    Mesh3DBody,
    SpotLight3D,
    ParticleEmitter
};

enum class Mesh3DShaderLoaderMapping {
    FXOutliner,
    FXBlink,
    FXTint,
};


class SceneLoader {
    std::map<std::string, SceneObjectLoaderMapping> sceneTypes = {
            {"Object3D", SceneObjectLoaderMapping::Object3D},
            {"Mesh3D", SceneObjectLoaderMapping::Mesh3D},
            {"LightPoint3D", SceneObjectLoaderMapping::LightPoint3D},
            {"SpotLight3D", SceneObjectLoaderMapping::SpotLight3D},
            {"Mesh3DGhost", SceneObjectLoaderMapping::Mesh3DGhost},
            {"Mesh3DBody", SceneObjectLoaderMapping::Mesh3DBody},
            {"ParticleEmitter", SceneObjectLoaderMapping::ParticleEmitter}
    };

    std::map<std::string, Mesh3DShaderLoaderMapping> mesh3DShaderTypes = {
            {"FXOutliner", Mesh3DShaderLoaderMapping::FXOutliner},
            {"FXBlink", Mesh3DShaderLoaderMapping::FXBlink},
            {"FXColorTint", Mesh3DShaderLoaderMapping::FXTint},
    };

public:
    explicit SceneLoader();

    void loadScene(const std::string& filename);

    static void saveScene(const std::string& filename);

    static Vertex3D parseVertex3DJSON(cJSON *vertex3DJSON);

    static void clearScene();

    static void createObjectInScene();

    static void createPointLight3DInScene();

    static void createMesh3DBodyToScene(const std::string &filename, const char *name);

    static void createGhostBody3DToScene(const std::string &filename, const char *name);

    std::map<std::string, SceneObjectLoaderMapping> &getSceneTypes();

    std::map<std::string, Mesh3DShaderLoaderMapping> &getMesh3DShaderTypes();

    static void createSpotLight3DInScene();

    static void createParticleEmitterInScene();

    static void createSprite3DInScene();

    static void createSprite2DInScene();

    static Color parseColorJSON(cJSON *color);
};


#endif //BRAKEZA3D_SCENELOADER_H
