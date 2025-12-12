//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBREADSCENESCRIPT_H
#define BRAKEZA3D_JOBREADSCENESCRIPT_H

#include "PendingJob.h"
#include "../Components/Components.h"
#include "../Misc/cJSON.h"

class JobReadSceneScript : public PendingJob
{
    cJSON *json;
public:
    JobReadSceneScript(cJSON *json)
    :
        PendingJob([this](){ process(); }, [this]() { callback(); }),
        json(cJSON_Duplicate(json, 1))
    {
    }

    void process()
    {
        Logging::Message("[Pools] JobReadSceneScript process");

        /*if (cJSON_GetObjectItemCaseSensitive(json, "scripts") != nullptr) {
            cJSON *currentScript;
            cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(json, "scripts")) {
                std::string fileName = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
                Components::get()->Scripting()->addSceneLUAScript(
                    new ScriptLUA(fileName, ScriptLUA::dataTypesFileFor(fileName))
                );
            }
        }*/
    }

    void callback()
    {
    }
};


#endif //BRAKEZA3D_JOBREADSCENESCRIPT_H