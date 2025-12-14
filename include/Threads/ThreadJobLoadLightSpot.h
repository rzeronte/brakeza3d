#ifndef BRAKEZA3D_JOBLOADLIGHTSPOT_H
#define BRAKEZA3D_JOBLOADLIGHTSPOT_H


#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../3D/LightSpot.h"
#include "../Misc/Logging.h"

class ThreadJobLoadLightSpot : public ThreadJobBase
{
    cJSON* json = nullptr;
    LightSpot* light = nullptr;
public:
    ThreadJobLoadLightSpot(LightSpot* light, cJSON* json)
    :
        json(cJSON_Duplicate(json, 1)),
        light(light)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        Logging::Message("[ThreadJobLoadLightPoint] Process END");
    }

    void fnCallback()
    {
        Brakeza::get()->addObject3D(light, light->getName());

        Logging::Message("[ThreadJobLoadLightPoint] Callback END");
    }

    ~ThreadJobLoadLightSpot()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};


#endif //BRAKEZA3D_JOBLOADLIGHTSPOT_H