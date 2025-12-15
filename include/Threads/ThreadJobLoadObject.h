#ifndef BRAKEZA3D_JOBLOADMESH3D_H
#define BRAKEZA3D_JOBLOADMESH3D_H

#include "ThreadJobBase.h"
#include "../Render/JSONSerializerRegistry.h"
#include "ThreadJobLoadMesh3D.h"

class ThreadJobLoadObject : public ThreadJobBase
{
    cJSON *json = nullptr;
    Object3D *object = nullptr;
public:
    explicit ThreadJobLoadObject(cJSON *json)
    :
        json(cJSON_Duplicate(json, 1))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        object = JSONSerializerRegistry::instance().deserialize(json);
        Logging::Message("[ThreadJobLoadObject] Process END");
    }

    void fnCallback()
    {
        Logging::Message("[ThreadJobLoadObject] Callback END");
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