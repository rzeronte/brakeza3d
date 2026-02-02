//
// Created by eduardo on 10/10/23.
//

#ifndef BRAKEZA3D_SCENELOADER_H
#define BRAKEZA3D_SCENELOADER_H

#include <string>
#include <map>
#include "../SceneObjectTypes.h"
#include "../Misc/cJSON.h"
#include "../Misc/FilePaths.h"

class Object3D;

class SceneLoader
{
    static const std::map<std::string, ObjectType> SceneObjectTypesMapping;

public:
    explicit SceneLoader();
    static bool isLoading;
    static bool isClearing;
    static void LoadSceneSettings(const cJSON *contentJSON);
    static void InitSerializers();
    static void LoadScene(const FilePath::SceneFile& filename);
    static void SaveScene(const FilePath::SceneFile& filename);
    static void CreateScene(const FilePath::SceneFile& filename);
    static void RemoveScene(const FilePath::SceneFile& filename);
    static void ClearScene();
    static void SceneLoaderCreateObject(cJSON *object);

    static void setLoading(bool cond){ isLoading = cond; }
};

#endif //BRAKEZA3D_SCENELOADER_H