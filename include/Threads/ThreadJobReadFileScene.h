#ifndef BRAKEZA3D_JOBREADFILESCENE_H
#define BRAKEZA3D_JOBREADFILESCENE_H

#include <string>
#include <utility>
#include "ThreadJobBase.h"
#include "ThreadJobReadSceneScript.h"
#include "ThreadJobReadSceneShaders.h"
#include "../Brakeza.h"
#include "../Misc/cJSON.h"
#include "../Misc/Logging.h"
#include "../Misc/Tools.h"
#include "../Misc/FilePaths.h"
#include "../Loaders/SceneLoader.h"
#include "../Loaders/Scene.h"
#include "ThreadJobResolveSwarms.h"

class ThreadJobReadFileScene : public ThreadJobBase
{
    FilePath::SceneFile filename;
    cJSON* json = nullptr;

    bool additive;
    bool loadScripts;
    bool loadShaders;
    bool loadCamera;
    bool loadRenderSettings;
    Scene *scene = nullptr;

public:
    explicit ThreadJobReadFileScene(
        const FilePath::SceneFile& file,
        Scene *scene            = nullptr,
        bool additive           = false,
        bool loadScripts        = true,
        bool loadShaders        = true,
        bool loadCamera         = true,
        bool loadRenderSettings = true
    )
    :
        filename(file),
        scene(scene),
        additive(additive),
        loadScripts(loadScripts),
        loadShaders(loadShaders),
        loadCamera(loadCamera),
        loadRenderSettings(loadRenderSettings)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };

        LOG_MESSAGE("[ThreadJobReadFileScene] Constructor - File: %s", filename.c_str());
    }

    void fnProcess()
    {
        LOG_MESSAGE("[ThreadJobReadFileScene] START - File: %s", filename.c_str());

        char *contentFile = Tools::ReadFile(filename);

        if (!contentFile) {
            LOG_ERROR("[ThreadJobReadFileScene] Failed to read file: %s", filename.c_str());
            return;
        }

        json = cJSON_Parse(contentFile);
        free(contentFile);

        LOG_MESSAGE("[ThreadJobReadFileScene] Process END");
    }

    void fnCallback()
    {
        if (loadCamera && loadRenderSettings) {
            SceneLoader::LoadSceneSettings(json);
        } else {
            if (loadCamera)         SceneLoader::LoadCameraSettings(json);
            if (loadRenderSettings) SceneLoader::LoadADSSettings(json);
        }

        auto objects = cJSON_GetObjectItemCaseSensitive(json, "objects");

        int objectCount = 0;
        cJSON *currentObject;
        cJSON_ArrayForEach(currentObject, objects) {
            Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(
                std::make_shared<ThreadJobLoadObject>(currentObject, true, scene)
            );
            objectCount++;
        }

        if (loadScripts)
            Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobReadSceneScript>(json, scene));

        if (loadShaders)
            Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobReadSceneShaders>(json, scene));

        Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobResolveSwarms>());

        if (!additive) {
            auto defaultPathScene = Config::get()->CONFIG_FOLDER + Config::get()->DEFAULT_SCENE;
            Components::get()->Scripting()->setCurrentScene(filename == defaultPathScene ? nullptr : scene);
        } else if (scene != nullptr) {
            Components::get()->Scripting()->addScene(scene);
        }

        SceneLoader::setLoading(false);
        LOG_MESSAGE("[ThreadJobReadFileScene] Callback END | Loaded %d objects.", objectCount);
    }

    ~ThreadJobReadFileScene()
    {
        if (json != nullptr) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_JOBREADFILESCENE_H
