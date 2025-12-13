#ifndef BRAKEZA3D_JOBREADFILESCENE_H
#define BRAKEZA3D_JOBREADFILESCENE_H

#include <string>
#include "PendingJob.h"
#include "../Brakeza.h"
#include "../Misc/cJSON.h"
#include "../Misc/Logging.h"
#include "../Misc/Tools.h"

class JobReadFileScene : public PendingJob {
    std::string filename;
    cJSON* json = nullptr;

public:
    explicit JobReadFileScene(const std::string& file)
    :
        filename(file)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };

        Logging::Message("[JobReadFileScene] Constructor - File: %s", filename.c_str());
    }

    void fnProcess()
    {
        Logging::Message("[Pools] JobReadFileScene::fnProcess START - File: %s", filename.c_str());

        size_t file_size = 0;
        auto contentFile = Tools::ReadFile(filename, file_size);

        if (!contentFile) {
            Logging::Error("[Pools] Failed to read file: %s", filename.c_str());
            return;
        }

        Logging::Message("[Pools] File read successfully, size: %zu bytes", file_size);

        json = cJSON_Parse(contentFile);

        if (!json) {
            Logging::Error("[Pools] Failed to parse JSON from file: %s", filename.c_str());
            const char* error = cJSON_GetErrorPtr();
            if (error) {
                Logging::Error("[Pools] JSON Parse Error: %s", error);
            }
        } else {
            Logging::Message("[Pools] JSON parsed successfully");
        }

        Logging::Message("[Pools] JobReadFileScene::fnProcess END");
    }

    void fnCallback()
    {
        Logging::Message("[Pools] JobReadFileScene::fnCallback START");

        SceneLoader::LoadSceneSettings(json);

        cJSON *objects = cJSON_GetObjectItemCaseSensitive(json, "objects");
        if (!objects) {
            Logging::Error("[Pools] No objects array found in scene JSON");
            return;
        }

        int objectCount = 0;
        cJSON *currentObject;
        cJSON_ArrayForEach(currentObject, objects) {
            Logging::Message("[Pools] Creating JobLoadObject #%d", objectCount);
            auto job = std::make_shared<JobLoadObject>(currentObject);
            Brakeza::get()->getPoolManager().Pool().enqueueWithMainThreadCallback(job);
            objectCount++;
        }

        Logging::Message("[Pools] JobReadFileScene::fnCallback END");
    }

    ~JobReadFileScene()
    {
        Logging::Message("[JobReadFileScene] Destructor - File: %s", filename.c_str());
        if (json != nullptr) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_JOBREADFILESCENE_H