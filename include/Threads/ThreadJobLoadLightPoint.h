#ifndef BRAKEZA3D_JOBLOADLIGHTPOINT_H
#define BRAKEZA3D_JOBLOADLIGHTPOINT


#include "ThreadJobBase.h"
#include "../3D/LightPoint.h"
#include "../Misc/Logging.h"

class ThreadJobLoadLightPoint : public ThreadJobBase
{
    cJSON* json = nullptr;
    LightPoint* light = nullptr;
public:
    ThreadJobLoadLightPoint(LightPoint* light, cJSON* json)
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

    ~ThreadJobLoadLightPoint()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};


#endif //BRAKEZA3D_JOBLOADLIGHTPOINT_H