//
// Created by eduardo on 10/10/23.
//

#ifndef BRAKEZA3D_SCENELOADER_H
#define BRAKEZA3D_SCENELOADER_H

#include <string>
#include <map>
#include "../Objects/Vertex3D.h"
#include "cJSON.h"

enum class SceneObjectLoaderMapping {
    Object3D,
    Mesh3D,
    LightPoint3D
};



class SceneLoader {
    std::map<std::string, SceneObjectLoaderMapping> sceneTypes = {
            {"Object3D", SceneObjectLoaderMapping::Object3D},
            {"Mesh3D", SceneObjectLoaderMapping::Mesh3D},
            {"LightPoint3D", SceneObjectLoaderMapping::LightPoint3D}
    };
public:
    explicit SceneLoader();
    void loadScene(const std::string& filename);
    void saveScene(const std::string& filename);

    static Vertex3D parseVertex3DJSON(cJSON *vertex3DJSON);

    void clearScene();

    static void createObjectInScene();

    static void createLightPointInScene();

    static void createMesh3DBodyToScene(const std::string &filename, const char *name);

    static void createGhostBody3DToScene(const std::string &filename, const char *name);
};


#endif //BRAKEZA3D_SCENELOADER_H
