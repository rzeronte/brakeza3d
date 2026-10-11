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
    // Carga en caliente (scripts ya en marcha): sin esto un script de escena nunca recibe onStart --
    // solo se llama al pulsar Play -- y su primer onUpdate corre con los globales sin inicializar.
    // Opcional para no cambiar el comportamiento de las cargas existentes (un proyecto puede tener el
    // mismo script como script de escena y de proyecto, y no debe arrancarlo dos veces).
    bool startScripts;
public:
    ThreadJobReadSceneScript(cJSON *json, Scene *scene = nullptr, bool startScripts = false)
    :
        json(cJSON_Duplicate(json, 1)),
        scene(scene),
        startScripts(startScripts)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
        if (scene != nullptr) scene->retainForJob();
    }

    void fnProcess()
    {
        LOG_MESSAGE("[ThreadJobReadSceneScript] Process END");
    }

    void fnCallback()
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
                if (startScripts && Components::get()->Scripting()->isExecuting()) {
                    script->RunGlobal("onStart");
                }
            }
        }
        LOG_MESSAGE("[ThreadJobReadSceneScript] Callback END");
    }

    ~ThreadJobReadSceneScript()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
        if (scene != nullptr) scene->releaseFromJob();
    }
};


#endif //BRAKEZA3D_JOBREADSCENESCRIPT_H
