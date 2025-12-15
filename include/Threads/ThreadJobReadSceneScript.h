//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBREADSCENESCRIPT_H
#define BRAKEZA3D_JOBREADSCENESCRIPT_H

#include "ThreadJobBase.h"
#include "../Components/Components.h"
#include "../Misc/cJSON.h"

class ThreadJobReadSceneScript : public ThreadJobBase
{
    cJSON *json;
public:
    ThreadJobReadSceneScript(cJSON *json)
    :
        json(cJSON_Duplicate(json, 1))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        if (cJSON_GetObjectItemCaseSensitive(json, "scripts") != nullptr) {
            cJSON *currentScript;
            cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(json, "scripts")) {
                std::string fileName = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
                Components::get()->Scripting()->addSceneLUAScript(
                    new ScriptLUA(fileName, ScriptLUA::dataTypesFileFor(fileName))
                );
            }
        }
        Logging::Message("[ThreadJobReadSceneScript] Process END");
    }

    void fnCallback()
    {
        Logging::Message("[ThreadJobReadSceneScript] Callback END");
    }

    ~ThreadJobReadSceneScript()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};


#endif //BRAKEZA3D_JOBREADSCENESCRIPT_H