//
// Created by eduardo on 10/10/23.
//

#ifndef BRAKEZA3D_SCENELOADER_H
#define BRAKEZA3D_SCENELOADER_H

#include <string>
#include <map>
#include "Misc/cJSON.h"

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
// IMPORTANT: The order must match EXACTLY the order of the enum
namespace SceneObjectTypes {
    inline constexpr const char* OBJECT_3D = "Object3D";
    inline constexpr const char* MESH_3D = "Mesh3D";
    inline constexpr const char* MESH_3D_ANIMATION = "Mesh3DAnimation";
    inline constexpr const char* LIGHT_POINT_3D = "LightPoint3D";
    inline constexpr const char* SPOT_LIGHT_3D = "SpotLight3D";
    inline constexpr const char* PARTICLE_EMITTER = "ParticleEmitter";
    inline constexpr const char* BILLBOARD_ANIMATION = "BillboardAnimation";
    inline constexpr const char* BILLBOARD_ANIMATION_8DIR = "BillboardAnimation8Directions";
    inline constexpr const char* IMAGE_2D_ANIMATION = "Image2DAnimation";
    inline constexpr const char* IMAGE_3D = "Image3D";
    inline constexpr const char* IMAGE_2D = "Image2D";
    inline constexpr const char* SWARM = "Swarm";
}

// IMPORTANT: The order must match EXACTLY the order of the enum
static constexpr const char* SceneObjectTypeNames[] = {
    SceneObjectTypes::OBJECT_3D,                // Object3D = 0
    SceneObjectTypes::MESH_3D,                  // Mesh3D = 1
    SceneObjectTypes::MESH_3D_ANIMATION,        // Mesh3DAnimation = 2
    SceneObjectTypes::LIGHT_POINT_3D,           // LightPoint3D = 3
    SceneObjectTypes::SPOT_LIGHT_3D,            // SpotLight3D = 4
    SceneObjectTypes::PARTICLE_EMITTER,         // ParticleEmitter = 5
    SceneObjectTypes::BILLBOARD_ANIMATION,      // BillboardAnimation = 6
    SceneObjectTypes::BILLBOARD_ANIMATION_8DIR, // BillboardAnimation8Directions = 7
    SceneObjectTypes::IMAGE_2D_ANIMATION,       // Image2DAnimation = 8
    SceneObjectTypes::IMAGE_3D,                 // Image3D = 9
    SceneObjectTypes::IMAGE_2D,                 // Image2D = 10
    SceneObjectTypes::SWARM                     // Swarm = 11
};

inline const char* getSceneObjectTypeName(SceneObjectLoaderMapping type) {
    return SceneObjectTypeNames[static_cast<size_t>(type)];
}

class Object3D;

class SceneLoader
{
    static const std::map<std::string, SceneObjectLoaderMapping> SceneObjectTypesMapping;

public:
    explicit SceneLoader();

    static void InitSerializers();
    static void LoadScene(const std::string& filename);
    static void SaveScene(const std::string& filename);
    static void CreateScene(const std::string &filename);
    static void RemoveScene(const std::string &filename);
    static void ClearScene();
    static void SceneLoaderCreateObject(cJSON *object);
};

#endif //BRAKEZA3D_SCENELOADER_H