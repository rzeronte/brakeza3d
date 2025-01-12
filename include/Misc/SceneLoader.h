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
    ParticleEmitter,
    BillboardAnimation,
    BillboardAnimation8Directions,
    Image2DAnimation,
    Image3D
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
            {"ParticleEmitter", SceneObjectLoaderMapping::ParticleEmitter},
            {"BillboardAnimation", SceneObjectLoaderMapping::BillboardAnimation},
            {"BillboardAnimation8Directions", SceneObjectLoaderMapping::BillboardAnimation8Directions},
            {"Image2DAnimation", SceneObjectLoaderMapping::Image2DAnimation},
            {"Image3D", SceneObjectLoaderMapping::Image3D}
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

    static void createMesh3DBodyToScene(const std::string &filename);

    static void createGhostBody3DToScene(const std::string &filename);

    static void createImage3DToScene(const std::string &filename);

    std::map<std::string, Mesh3DShaderLoaderMapping> &getMesh3DShaderTypes();

    static void createSpotLight3DInScene();

    static void createParticleEmitterInScene();

    static void createBillboardAnimationInScene(const std::string& filename);

    static void createImage2DAnimatedInScene(const std::string& filename);

    static void createImage2DInScene(const std::string& filename);

    static Color parseColorJSON(cJSON *color);

    static void createBillboardAnimation8Directions();
};

#endif //BRAKEZA3D_SCENELOADER_H
