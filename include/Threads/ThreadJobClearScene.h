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
        auto scripting = Components::get()->Scripting();

        scripting->StopLUAScripts();

        for (auto &o: scripting->getSceneLUAScripts()) {
            scripting->RemoveSceneScript(o);
        }

        scripting->getProjectScenes().clear();

        auto render = Components::get()->Render();
        for (auto &s: render->getSceneShaders()) {
            render->RemoveSceneShader(s);
        }

        for (auto &object: Brakeza::get()->getSceneObjects()) {
            if (!object->isMultiScene()) {
                object->setRemoved(true);
            }
        }

        Components::get()->Render()->setSelectedObject(nullptr);

        SceneLoader::isClearing = false;
        LOG_MESSAGE("[ThreadJobClearScene] Process END");
    }

    void fnCallback()
    {
        LOG_MESSAGE("[ThreadJobClearScene] Callback END");
    }
};

#endif //BRAKEZA3D_THREADJOBCLEARSCENE_H