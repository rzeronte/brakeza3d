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
