#ifndef BRAKEZA3D_JOBREADFILESCENE_H
#define BRAKEZA3D_JOBREADFILESCENE_H

#include <string>

#include "JobParseScene.h"
#include "JobReadSceneScript.h"
#include "JobReadSceneShaders.h"
#include "PendingJob.h"
#include "../Brakeza.h"
#include "../Misc/cJSON.h"
#include "../Misc/Logging.h"

class JobReadFileScene : public PendingJob {
    std::string filename;
    cJSON* json = nullptr;

public:
    JobReadFileScene(std::string filename)
    :
        PendingJob([this](){ process(); }, [this]() { callback(); }),
        filename(filename)
    {
    }

    void process()
    {
        Logging::Message("[Pools] JobReadFileScene process");

        size_t file_size;
        auto contentFile = Tools::ReadFile(filename, file_size);
        json = cJSON_Parse(contentFile);
    }

    void callback()
    {
        Logging::Message("[Pools] JobReadFileScene callback");

        if (json != nullptr) {

            auto jobScripts = std::make_shared<JobReadSceneScript>(json);
            Brakeza::get()->getPoolManager().getIOPool().enqueue(jobScripts);

            auto jobShaders = std::make_shared<JobReadSceneShaders>(json);
            Brakeza::get()->getPoolManager().getIOPool().enqueue(jobShaders);

            auto job = std::make_shared<JobParseScene>(json);
            Brakeza::get()->getPoolManager().getIOPool().enqueueWithMainThreadCallback(job);

            cJSON_Delete(json);
            json = nullptr;
        }
    }

    ~JobReadFileScene() {
        if (json != nullptr) {
            cJSON_Delete(json);
        }
    }

};

#endif