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

#ifndef BRAKEZA3D_THREADJOBCLEARSCENE_H
#define BRAKEZA3D_THREADJOBCLEARSCENE_H

#include "ThreadJobBase.h"

class ThreadJobClearScene : public ThreadJobBase
{
public:
    ThreadJobClearScene()
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        // Vacío: todo el trabajo se hace en fnCallback (main thread).
    }

    void fnCallback()
    {
        auto scripting = Components::get()->Scripting();

        scripting->StopLUAScripts();

        auto scriptsCopy = scripting->getSceneScripts();
        for (auto *o: scriptsCopy) {
            scripting->RemoveSceneScript(o);
        }

        auto render = Components::get()->Render();
        auto shadersCopy = render->getSceneShaders();
        for (auto *s: shadersCopy) {
            render->RemoveSceneShader(s);
        }

        auto sceneObjects = Brakeza::get()->copySceneObjects();
        for (auto &object: sceneObjects) {
            if (!object->isMultiScene()) {
                object->setRemoved(true);
            }
        }

        Components::get()->Scripting()->setCurrentScene(nullptr);
        Components::get()->Render()->setSelectedObject(nullptr);
        Components::get()->Render()->clearEngineCache();

        SceneLoader::isClearing.store(false);
        LOG_MESSAGE("[ThreadJobClearScene] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBCLEARSCENE_H