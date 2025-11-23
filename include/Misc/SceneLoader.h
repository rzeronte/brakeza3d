//
// Created by eduardo on 10/10/23.
//

#ifndef BRAKEZA3D_SCENELOADER_H
#define BRAKEZA3D_SCENELOADER_H

#include <string>
#include <map>

#include "cJSON.h"


enum class SceneObjectLoaderMapping {
    Object3D,
    Mesh3D,
    Mesh3DAnimation,
    LightPoint3D,
    SpotLight3D,
    ParticleEmitter,
    BillboardAnimation,
    BillboardAnimation8Directions,
    Image2DAnimation,
    Image3D,
    Image2D
};

static std::map<std::string, SceneObjectLoaderMapping> SceneObjectTypesMapping = {
    {"Object3D", SceneObjectLoaderMapping::Object3D},
    {"Mesh3D", SceneObjectLoaderMapping::Mesh3D},
    {"Mesh3DAnimation", SceneObjectLoaderMapping::Mesh3DAnimation},
    {"LightPoint3D", SceneObjectLoaderMapping::LightPoint3D},
    {"SpotLight3D", SceneObjectLoaderMapping::SpotLight3D},
    {"ParticleEmitter", SceneObjectLoaderMapping::ParticleEmitter},
    {"BillboardAnimation", SceneObjectLoaderMapping::BillboardAnimation},
    {"BillboardAnimation8Directions", SceneObjectLoaderMapping::BillboardAnimation8Directions},
    {"Image2DAnimation", SceneObjectLoaderMapping::Image2DAnimation},
    {"Image3D", SceneObjectLoaderMapping::Image3D},
    {"Image2D", SceneObjectLoaderMapping::Image2D}
};

class Object3D;

class SceneLoader {
public:
    explicit SceneLoader();

    static void InitSerializers();
    static void LoadScene(const std::string& filename);
    static void SaveScene(const std::string& filename);
    static void CreateScene(const std::string &filename);
    static void RemoveScene(const std::string &filename);
    static void ClearScene();

    static void createMesh3DAnimationToScene(const std::string& animationFile);
    static void createImage3DToScene(const std::string &filename);
    static void createPointLight3DInScene();
    static void createSpotLight3DInScene();
    static void createImage2DInScene(const std::string& filename);
    static void createParticleEmitterInScene();
    static void createImage2DAnimatedInScene(const std::string& filename);
    static void createBillboardAnimationInScene(const std::string& filename);
    static void createBillboardAnimation8Directions();

    static void SceneLoaderCreateObject(cJSON *object);

};

#endif //BRAKEZA3D_SCENELOADER_H
