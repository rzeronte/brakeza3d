//
// Created by Eduardo on 15/12/2025.
//

#ifndef BRAKEZA3D_THREADJOBCLEANWORLD_H
#define BRAKEZA3D_THREADJOBCLEANWORLD_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"

class ThreadJobCleanWorld : public ThreadJobBase
{
public:
    ThreadJobCleanWorld()
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        for (auto &o: Brakeza::get()->getSceneObjects()) {
            if (!o->isMultiScene() && !o->isBelongToScene()) {
                o->setRemoved(true);
            }
        }

        LOG_MESSAGE("[ThreadJobCleanWorld] Process END");
    }

    void fnCallback()
    {
        LOG_MESSAGE("[ThreadJobCleanWorld] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBCLEANWORLD_H
