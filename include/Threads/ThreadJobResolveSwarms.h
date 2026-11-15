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
