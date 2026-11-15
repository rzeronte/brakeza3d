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
        LOG_MESSAGE("[ThreadJobCleanScene] Process END");
    }

    void fnCallback()
    {
        auto sceneObjects = Brakeza::get()->copySceneObjects();
        for (auto &o: sceneObjects) {
            if (!o->isMultiScene() && !o->isBelongToScene()) {
                o->setRemoved(true);
            }
        }
        LOG_MESSAGE("[ThreadJobCleanScene] Callback END");
    }
};

#endif