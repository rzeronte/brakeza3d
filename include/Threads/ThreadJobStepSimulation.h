//
// Created by Eduardo on 15/12/2025.
//

#ifndef BRAKEZA3D_THREADJOBSTEPSIMULATION_H
#define BRAKEZA3D_THREADJOBSTEPSIMULATION_H

#include "ThreadJobBase.h"
#include "../Components/Components.h"

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
        Components::get()->Collisions()->StepSimulation(dt);
    }

    void fnCallback()
    {
    }
};
#endif //BRAKEZA3D_THREADJOBSTEPSIMULATION_H