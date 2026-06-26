//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBREADSCENESHADERS_H
#define BRAKEZA3D_JOBREADSCENESHADERS_H

#include "ThreadJobBase.h"
#include "../Components/Components.h"
#include "../GUI/Objects/ShadersGUI.h"
#include "../Misc/cJSON.h"
#include "../OpenGL/Code/ShaderOGLCustomCodePostprocessing.h"
#include "../OpenGL/Code/ShaderBaseCustomOGLCode.h"
#include "../Loaders/Scene.h"

class ThreadJobReadSceneShaders : public ThreadJobBase
{
    cJSON *json;
    Scene *scene;
public:
    ThreadJobReadSceneShaders(cJSON *json, Scene *scene = nullptr)
    :
        json(cJSON_Duplicate(json, 1)),
        scene(scene)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        LOG_MESSAGE("[ThreadJobReadSceneShaders] Process END");
    }

    void fnCallback()
    {
        cJSON *currentShaderJSON;
        cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(json, "shaders")) {
            auto typesFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "typesFile")->valuestring;
            auto shader = Components::get()->Render()->LoadShaderIntoScene(typesFile);

            if (shader) {
                shader->setScene(scene);

                auto typesArray = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");
                if (typesArray) {
                    if (auto* codeShader = dynamic_cast<ShaderBaseCustomOGLCode*>(shader)) {
                        codeShader->overrideDataTypesFromJSON(typesArray);
                    }
                }

                auto enabledItem = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "enabled");
                if (cJSON_IsBool(enabledItem)) {
                    shader->setEnabled(cJSON_IsTrue(enabledItem));
                }
            }
        }

        LOG_MESSAGE("[ThreadJobReadSceneShaders] Callback END");
    }
};


#endif //BRAKEZA3D_JOBREADSCENESHADERS_H
