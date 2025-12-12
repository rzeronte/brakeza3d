//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBSCENELOADER_H
#define BRAKEZA3D_JOBSCENELOADER_H

#include "PendingJob.h"
#include "../Loaders/SceneLoader.h"
#include "../Misc/cJSON.h"
#include "../Misc/Logging.h"


class JobParseScene : public PendingJob {
    cJSON *json;
public:
    JobParseScene(cJSON *json)
    :
        PendingJob([&](){ process(); }, [&]() { callback(); }),
        json(cJSON_Duplicate(json, 1))
    {
    }

    void process()
    {
        Logging::Message("[Pools] JobParseScene process");
        SceneLoader::LoadSceneSettings(json);

    }

    void callback()
    {
        cJSON *currentObject;
        cJSON_ArrayForEach(currentObject, cJSON_GetObjectItemCaseSensitive(json, "objects")) {
            SceneLoader::SceneLoaderCreateObject(currentObject);
        }

        Logging::Message("Pues esto ya es el callback de JobParseScene!!!!");
    }
};


#endif //BRAKEZA3D_JOBSCENELOADER_H