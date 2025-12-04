//
// Created by eduardo on 10/10/23.
//

#ifndef BRAKEZA3D_SCENELOADER_H
#define BRAKEZA3D_SCENELOADER_H

#include <string>
#include <map>
#include "../SceneObjectTypes.h"
#include "../Misc/cJSON.h"

class Object3D;

class SceneLoader
{
    static const std::map<std::string, ObjectTypes> SceneObjectTypesMapping;

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