//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBREADSCENESHADERS_H
#define BRAKEZA3D_JOBREADSCENESHADERS_H

#include "PendingJob.h"
#include "../Components/Components.h"
#include "../Misc/cJSON.h"
#include "../OpenGL/ShaderOGLCustomPostprocessing.h"

class JobReadSceneShaders : public PendingJob
{
    cJSON *json;
public:
    JobReadSceneShaders(cJSON *json)
    :
        PendingJob([this](){ process(); }, [this]() { callback(); }),
        json(cJSON_Duplicate(json, 1))
    {
    }

    void process()
    {
        Logging::Message("[Pools] JobReadSceneShaders callback");

        /*cJSON *currentShaderJSON;
        cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(json, "shaders")) {
            auto name = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;
            auto vertex = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "vertexshader")->valuestring;
            auto fragment = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "fragmentshader")->valuestring;
            auto types = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");

            auto shader = new ShaderOGLCustomPostprocessing(name, vertex, fragment, types);
            Components::get()->Render()->addShaderToScene(shader);
        }*/
    }

    void callback()
    {
    }


};


#endif //BRAKEZA3D_JOBREADSCENESHADERS_H