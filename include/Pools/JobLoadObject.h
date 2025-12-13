#ifndef BRAKEZA3D_JOBLOADMESH3D_H
#define BRAKEZA3D_JOBLOADMESH3D_H

#include "PendingJob.h"
#include "../Serializers/JSONSerializerRegistry.h"
#include "JobLoadMesh3D.h"

class JobLoadObject : public PendingJob
{
    cJSON *json = nullptr;
    Object3D *object = nullptr;
public:
    explicit JobLoadObject(cJSON *objectJson)
    :
        json(cJSON_Duplicate(objectJson, 1))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        object = JSONSerializerRegistry::instance().deserialize(json);
    }

    void fnCallback()
    {
    }

    ~JobLoadObject()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_JOBLOADMESH3D_H