//
// Created by Eduardo on 15/12/2025.
//

#ifndef BRAKEZA3D_THREADJOBCLEARWORLD_H
#define BRAKEZA3D_THREADJOBCLEARWORLD_H

#include "ThreadJobBase.h"

class ThreadJobClearWorld : public ThreadJobBase
{
public:
    ThreadJobClearWorld()
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        // Vacío: todo el trabajo se hace en fnCallback (main thread) para evitar
        // accesos a Lua/OpenGL/Bullet desde worker thread.
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

        SceneLoader::isClearing.store(false);
        LOG_MESSAGE("[ThreadJobClearWorld] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBCLEARWORLD_H
