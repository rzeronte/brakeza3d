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
#include "../Loaders/SceneLoader.h"

class ThreadJobReadFileScene : public ThreadJobBase
{
    std::string filename;
    cJSON* json = nullptr;

public:
    explicit ThreadJobReadFileScene(std::string file)
    :
        filename(std::move(file))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };

        LOG_MESSAGE("[ThreadJobReadFileScene] Constructor - File: %s", filename.c_str());
    }

    void fnProcess()
    {
        LOG_MESSAGE("[ThreadJobReadFileScene] START - File: %s", filename.c_str());

        auto contentFile = Tools::ReadFile(filename);

        if (!contentFile) {
            LOG_ERROR("[ThreadJobReadFileScene] Failed to read file: %s", filename.c_str());
            return;
        }

        json = cJSON_Parse(contentFile);

        LOG_MESSAGE("[ThreadJobReadFileScene] Process END");
    }

    void fnCallback()
    {
        SceneLoader::LoadSceneSettings(json);

        auto objects = cJSON_GetObjectItemCaseSensitive(json, "objects");

        int objectCount = 0;
        cJSON *currentObject;
        cJSON_ArrayForEach(currentObject, objects) {
            Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadObject>(currentObject));
            objectCount++;
        }

        Brakeza::get()->PoolCompute().enqueue(std::make_shared<ThreadJobReadSceneScript>(json));

        Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobReadSceneShaders>(json));

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