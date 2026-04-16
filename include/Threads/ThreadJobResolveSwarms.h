//
// Created by Eduardo on 16/04/2026.
//

#ifndef BRAKEZA3D_THREADJOBRESOLVESWARMS_H
#define BRAKEZA3D_THREADJOBRESOLVESWARMS_H

#include "ThreadJobBase.h"
#include "../3D/Swarm.h"
#include "../Brakeza.h"
#include "../Misc/Logging.h"

class ThreadJobResolveSwarms : public ThreadJobBase
{
public:
    ThreadJobResolveSwarms()
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess() {}

    void fnCallback()
    {
        int resolved = 0;
        for (auto* obj : Brakeza::get()->getSceneObjects()) {
            auto* swarm = dynamic_cast<Swarm*>(obj);
            if (swarm) {
                swarm->resolvePendingMembers();
                resolved++;
            }
        }
        LOG_MESSAGE("[ThreadJobResolveSwarms] Resolved %d swarm(s)", resolved);
    }
};

#endif //BRAKEZA3D_THREADJOBRESOLVESWARMS_H
