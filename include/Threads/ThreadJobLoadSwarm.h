/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by Eduardo on 16/04/2026.
//

#ifndef BRAKEZA3D_THREADJOBLOADSWARM_H
#define BRAKEZA3D_THREADJOBLOADSWARM_H

#include "ThreadJobBase.h"
#include "../3D/Swarm.h"
#include "../Misc/cJSON.h"
#include "../Misc/Logging.h"

class ThreadJobLoadSwarm : public ThreadJobBase
{
    cJSON* json = nullptr;
    Swarm* swarm = nullptr;
public:
    ThreadJobLoadSwarm(Swarm* swarm, cJSON* json)
    :
        json(cJSON_Duplicate(json, 1)),
        swarm(swarm)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        LOG_MESSAGE("[ThreadJobLoadSwarm] Process END");
    }

    void fnCallback()
    {
        Brakeza::get()->AddObject3D(swarm, swarm->getName());
        LOG_MESSAGE("[ThreadJobLoadSwarm] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBLOADSWARM_H
