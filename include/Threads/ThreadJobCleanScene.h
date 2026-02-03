//
// Created by Eduardo on 15/12/2025.
//

#ifndef BRAKEZA3D_THREADJOBCLEANSCENE_H
#define BRAKEZA3D_THREADJOBCLEANSCENE_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"

class ThreadJobCleanScene : public ThreadJobBase
{
public:
    ThreadJobCleanScene()
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

        LOG_MESSAGE("[ThreadJobCleanScene] Process END");
    }

    void fnCallback()
    {
        LOG_MESSAGE("[ThreadJobCleanScene] Callback END");
    }
};

#endif