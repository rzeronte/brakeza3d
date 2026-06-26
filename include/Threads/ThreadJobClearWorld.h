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
        auto scripting = Components::get()->Scripting();

        scripting->StopLUAScripts();

        for (auto &o: scripting->getSceneScripts()) {
            scripting->RemoveSceneScript(o);
        }

        auto render = Components::get()->Render();
        for (auto &s: render->getSceneShaders()) {
            render->RemoveSceneShader(s);
        }

        for (auto &object: Brakeza::get()->getSceneObjects()) {
            if (!object->isMultiScene()) {
                object->setRemoved(true);
            }
        }

        Components::get()->Scripting()->setCurrentScene(nullptr);
        Components::get()->Render()->setSelectedObject(nullptr);

        SceneLoader::isClearing = false;
        LOG_MESSAGE("[ThreadJobClearWorld] Process END");
    }

    void fnCallback()
    {
        LOG_MESSAGE("[ThreadJobClearWorld] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBCLEARWORLD_H
