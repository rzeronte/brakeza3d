/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
        LOG_MESSAGE("[ThreadJobLoadLightPoint] Process END");
    }

    void fnCallback()
    {
        Brakeza::get()->AddObject3D(light, light->getName());

        LOG_MESSAGE("[ThreadJobLoadLightPoint] Callback END");
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