#ifndef BRAKEZA3D_JOBLOADMESH3D_H
#define BRAKEZA3D_JOBLOADMESH3D_H

#include "ThreadJobBase.h"
#include "../Render/JSONSerializerRegistry.h"

class ThreadJobLoadObject : public ThreadJobBase
{
    cJSON *json = nullptr;
    Object3D *object = nullptr;
    bool isBelongingToScene = false;
public:
    ThreadJobLoadObject(cJSON *json, bool is_belonging_to_scene = false)
    :
        json(cJSON_Duplicate(json, 1)),
        isBelongingToScene(is_belonging_to_scene)
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