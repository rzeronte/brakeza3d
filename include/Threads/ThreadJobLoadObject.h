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
    }

    void fnProcess()
    {
        object = JSONSerializerRegistry::instance().deserialize(json);
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
    }
};

#endif //BRAKEZA3D_JOBLOADMESH3D_H
