//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBREADSCENESHADERS_H
#define BRAKEZA3D_JOBREADSCENESHADERS_H

#include "ThreadJobBase.h"
#include "../Components/Components.h"
#include "../Misc/cJSON.h"
#include "../OpenGL/ShaderOGLCustomPostprocessing.h"

class ThreadJobReadSceneShaders : public ThreadJobBase
{
    cJSON *json;
public:
    ThreadJobReadSceneShaders(cJSON *json)
    :
        json(cJSON_Duplicate(json, 1))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        cJSON *currentShaderJSON;
        cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(json, "shaders")) {
            auto vsFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "vsFile")->valuestring;
            auto fsFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "fsFile")->valuestring;
            auto typesFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "typesFile")->valuestring;
            auto dataTypes = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");
            auto name = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;

            auto shader = new ShaderOGLCustomPostprocessing(name, typesFile, vsFile, fsFile, dataTypes);
            shader->PrepareBackground();
            Components::get()->Render()->AddShaderToScene(shader);
        }
        LOG_MESSAGE("[ThreadJobReadFileScene] Process END");
    }

    void fnCallback()
    {
        auto &shaders = Components::get()->Render()->getSceneShaders();
        for (auto &s : shaders) {
            s->PrepareMainThread();
        }
        LOG_MESSAGE("[ThreadJobReadFileScene] Callback END");
    }


};


#endif //BRAKEZA3D_JOBREADSCENESHADERS_H