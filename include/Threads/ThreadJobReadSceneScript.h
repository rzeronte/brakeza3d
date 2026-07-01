//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBREADSCENESCRIPT_H
#define BRAKEZA3D_JOBREADSCENESCRIPT_H

#include "ThreadJobBase.h"
#include "../Components/Components.h"
#include "../Misc/cJSON.h"
#include "../Loaders/Scene.h"

class ThreadJobReadSceneScript : public ThreadJobBase
{
    cJSON *json;
    Scene *scene;
public:
    ThreadJobReadSceneScript(cJSON *json, Scene *scene = nullptr)
    :
        json(cJSON_Duplicate(json, 1)),
        scene(scene)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        if (cJSON_GetObjectItemCaseSensitive(json, "scripts") != nullptr) {
            cJSON *currentScript;
            cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(json, "scripts")) {
                std::string codeFile  = cJSON_GetObjectItemCaseSensitive(currentScript, "codeFile")->valuestring;
                std::string typesFile = cJSON_GetObjectItemCaseSensitive(currentScript, "typesFile")->valuestring;
                std::string name      = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
                auto dataTypes        = cJSON_GetObjectItemCaseSensitive(currentScript, "types");

                if (!Tools::FileExists(codeFile.c_str()) || !Tools::FileExists(typesFile.c_str())) {
                    LOG_ERROR("[ThreadJobReadSceneScript] Error loading script files '%s' and '%s'", codeFile.c_str(), typesFile.c_str());
                    return;
                }

                auto *script = new ScriptLUA(name, codeFile, typesFile, dataTypes);

                cJSON *tpsJSON = cJSON_GetObjectItemCaseSensitive(currentScript, "ticks_per_second");
                if (tpsJSON && cJSON_IsNumber(tpsJSON)) {
                    script->setTicksPerSecond(tpsJSON->valueint);
                }

                script->setScene(scene);
                Components::get()->Scripting()->AddSceneLUAScript(script);
            }
        }
        LOG_MESSAGE("[ThreadJobReadSceneScript] Process END");
    }

    void fnCallback()
    {
        LOG_MESSAGE("[ThreadJobReadSceneScript] Callback END");
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
