#ifndef BRAKEZA3D_JOBREADFILESCENE_H
#define BRAKEZA3D_JOBREADFILESCENE_H

#include <string>
#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../Misc/cJSON.h"
#include "../Misc/Logging.h"
#include "../Misc/Tools.h"

class ThreadJobReadFileScene : public ThreadJobBase {
    std::string filename;
    cJSON* json = nullptr;

public:
    explicit ThreadJobReadFileScene(const std::string& file)
    :
        filename(file)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };

        Logging::Message("[ThreadJobReadFileScene] Constructor - File: %s", filename.c_str());
    }

    void fnProcess()
    {
        Logging::Message("[Pools] ThreadJobReadFileScene::fnProcess START - File: %s", filename.c_str());

        size_t file_size = 0;
        auto contentFile = Tools::ReadFile(filename, file_size);

        if (!contentFile) {
            Logging::Error("[Pools] Failed to read file: %s", filename.c_str());
            return;
        }

        json = cJSON_Parse(contentFile);

        Logging::Message("[ThreadJobReadFileScene] Process END");
    }

    void fnCallback()
    {
        SceneLoader::LoadSceneSettings(json);

        cJSON *objects = cJSON_GetObjectItemCaseSensitive(json, "objects");
        if (!objects) {
            Logging::Error("[Pools] No objects array found in scene JSON");
            return;
        }

        int objectCount = 0;
        cJSON *currentObject;
        cJSON_ArrayForEach(currentObject, objects) {
            Logging::Message("[Pools] Creating ThreadJobLoadObject #%d", objectCount);
            auto job = std::make_shared<ThreadJobLoadObject>(currentObject);
            Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(job);
            objectCount++;
        }

        Logging::Message("[ThreadJobReadFileScene] Callback END");
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