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

        LOG_MESSAGE("[ThreadJobReadFileScene] Process END");
    }

    void fnCallback()
    {
        cJSON *currentShaderJSON;
        cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(json, "shaders")) {
            auto typesFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "typesFile")->valuestring;
            Components::get()->Render()->LoadShaderIntoScene(typesFile);

        }

        /*auto &shaders = Components::get()->Render()->getSceneShaders();
        for (auto &s : shaders) {
            s->PrepareMainThread();
        }*/
        LOG_MESSAGE("[ThreadJobReadFileScene] Callback END");
    }


};


#endif //BRAKEZA3D_JOBREADSCENESHADERS_H