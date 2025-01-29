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
    SpotLight3D,
    ParticleEmitter,
    BillboardAnimation,
    BillboardAnimation8Directions,
    Image2DAnimation,
    Image3D,
    Image2D
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
            {"ParticleEmitter", SceneObjectLoaderMapping::ParticleEmitter},
            {"BillboardAnimation", SceneObjectLoaderMapping::BillboardAnimation},
            {"BillboardAnimation8Directions", SceneObjectLoaderMapping::BillboardAnimation8Directions},
            {"Image2DAnimation", SceneObjectLoaderMapping::Image2DAnimation},
            {"Image3D", SceneObjectLoaderMapping::Image3D},
            {"Image2D", SceneObjectLoaderMapping::Image2D}
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

    static void clearScene();

    static void createObjectInScene();

    static void createPointLight3DInScene();

    static void createImage3DToScene(const std::string &filename);

    std::map<std::string, Mesh3DShaderLoaderMapping> &getMesh3DShaderTypes();

    static void createSpotLight3DInScene();

    static void createParticleEmitterInScene();

    static void createBillboardAnimationInScene(const std::string& filename);

    static void createImage2DAnimatedInScene(const std::string& filename);

    static void createImage2DInScene(const std::string& filename);

    static void createBillboardAnimation8Directions();

    static void createMesh3D(const std::string& animationFile);

    static void createMesh3DAnimationToScene(const std::string& animationFile);

    static void createScene(const std::string &filename);

    static void removeScene(const std::string &filename);

};

#endif //BRAKEZA3D_SCENELOADER_H
