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
// Created by Eduardo on 15/12/2025.
//

#ifndef BRAKEZA3D_THREADJOBSTEPSIMULATION_H
#define BRAKEZA3D_THREADJOBSTEPSIMULATION_H

#include "ThreadJobBase.h"
#include "../Components/Components.h"
#include "../Render/ProfilerZones.h"

class ThreadJobStepSimulation : public ThreadJobBase
{
    float dt = 0;
public:
    ThreadJobStepSimulation(float dt)
    :
        dt(dt)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        BZ_ZONE_N("Physics StepSimulation");
        Components::get()->Collisions()->StepSimulation(dt);
    }

    void fnCallback()
    {

    }
};

#endif //BRAKEZA3D_THREADJOBSTEPSIMULATION_H