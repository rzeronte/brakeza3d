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
        LOG_MESSAGE("[ThreadJobCleanWorld] Process END");
    }

    void fnCallback()
    {
        auto sceneObjects = Brakeza::get()->copySceneObjects();
        for (auto &o: sceneObjects) {
            if (!o->isMultiScene() && !o->isBelongToScene()) {
                o->setRemoved(true);
            }
        }
        LOG_MESSAGE("[ThreadJobCleanWorld] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBCLEANWORLD_H
