/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_JOBLOADMESH3D_H
#define BRAKEZA3D_JOBLOADMESH3D_H

#include "ThreadJobBase.h"
#include "../Render/JSONSerializerRegistry.h"
#include "../Loaders/Scene.h"

class ThreadJobLoadObject : public ThreadJobBase
{
    cJSON *json = nullptr;
    Object3D *object = nullptr;
    bool isBelongingToScene = false;
    Scene *scene = nullptr;
public:
    ThreadJobLoadObject(cJSON *json, bool is_belonging_to_scene = false, Scene *scene = nullptr)
    :
        json(cJSON_Duplicate(json, 1)),
        isBelongingToScene(is_belonging_to_scene),
        scene(scene)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
        if (scene != nullptr) scene->retainForJob();
    }

    void fnProcess()
    {
        object = JSONSerializerRegistry::instance().deserialize(json);
        if (object == nullptr) {
            LOG_ERROR("[ThreadJobLoadObject] deserialize returned null, skipping");
            return;
        }
        object->setBelongToScene(isBelongingToScene);
        LOG_MESSAGE("[ThreadJobLoadObject] Process END");
    }

    void fnCallback()
    {
        if (scene != nullptr && object != nullptr) {
            object->setScene(scene);
            scene->addObject(object);
        }
        LOG_MESSAGE("[ThreadJobLoadObject] Callback END");
    }

    ~ThreadJobLoadObject()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
        if (scene != nullptr) scene->releaseFromJob();
    }
};

#endif //BRAKEZA3D_JOBLOADMESH3D_H
