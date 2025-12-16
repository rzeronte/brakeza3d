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
                std::string codeFile = cJSON_GetObjectItemCaseSensitive(currentScript, "codeFile")->valuestring;
                std::string typesFile = cJSON_GetObjectItemCaseSensitive(currentScript, "typesFile")->valuestring;
                std::string name = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
                auto dataTypes = cJSON_GetObjectItemCaseSensitive(currentScript, "types");

                if (!Tools::FileExists(codeFile.c_str()) || !Tools::FileExists(typesFile.c_str())) {
                    Logging::Error("[ThreadJobReadSceneScript] Error loading script files '%s' and '%s'", codeFile.c_str(), typesFile.c_str());
                    return;
                }

                Components::get()->Scripting()->addSceneLUAScript(new ScriptLUA(name, codeFile, typesFile, dataTypes));
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