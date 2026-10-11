//
// Created by eduardo on 10/10/23.
//

#ifndef BRAKEZA3D_SCENELOADER_H
#define BRAKEZA3D_SCENELOADER_H

#include <string>
#include <map>
#include <atomic>
#include "../SceneObjectTypes.h"
#include "../Misc/cJSON.h"
#include "../Misc/FilePaths.h"

class Object3D;
class Scene;

class SceneLoader
{
    static const std::map<std::string, ObjectType> SceneObjectTypesMapping;

public:
    explicit SceneLoader();
    static std::atomic<bool> isLoading;
    static std::atomic<bool> isClearing;

    static void LoadSceneSettings(const cJSON *contentJSON);
    static void LoadCameraSettings(const cJSON *contentJSON);
    static void LoadADSSettings(const cJSON *contentJSON);
    static void InitSerializers();

    // Asociación escena↔fichero de luces FBX (ver FBXLightLoader). "currentLightsFile" es el
    // path actualmente asociado a la escena cargada -- se guarda como campo "lightsFile" en el
    // JSON de la escena (SaveScene) y se usa para reimportar sin tener que ir al menú cada vez.
    static std::string currentLightsFile;
    // Atenuación opcional para las luces de ese fichero (campo "lightsAttenuation" de la escena:
    // { constant, linear, quadratic }). FBXLightLoader pone siempre la tabla por defecto (1, 0.09,
    // 0.032), pensada para distancias cortas: en un interior escalado (p.ej. ~9 u/m) el suelo apenas
    // recibe luz. Sin el campo, se respeta la del cargador.
    static bool hasLightsAttenuation;
    static float lightsAttenuation[3];
    static void LoadLightsFileAssociation(const cJSON *contentJSON, Scene *scene);
    static void ReimportLightsFile(const std::string &path, Scene *scene = nullptr);

    static void LoadScene(const FilePath::SceneFile& filename);
    static void LoadSceneAdditive(
        const FilePath::SceneFile& filename,
        bool loadScripts        = false,
        bool loadShaders        = false,
        bool loadCamera         = false,
        bool loadRenderSettings = false,
        bool startScripts       = false     // con los scripts ya en marcha: onStart de cada script de escena al registrarlo
    );

    static void setSceneActive(const std::string& name, bool active);
    static void setSceneHidden(const std::string& name, bool hidden);
    static void UnloadScene(const std::string& name);
    static void ReloadScene(const std::string& name);

    static void SaveScene(const FilePath::SceneFile& filename);
    static void CreateScene(const FilePath::SceneFile& filename);
    static void RemoveScene(const FilePath::SceneFile& filename);
    static void ClearWorld();
    static void CleanWorld();
    static void SceneLoaderCreateObject(cJSON *object);
    static void setLoading(bool cond){ isLoading = cond; }
};

#endif //BRAKEZA3D_SCENELOADER_H
