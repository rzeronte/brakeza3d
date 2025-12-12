//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBLOADMESH3D_H
#define BRAKEZA3D_JOBLOADMESH3D_H

#include "PendingJob.h"
#include "../Serializers/JSONSerializerRegistry.h"

class JobLoadObject : public PendingJob
{
    cJSON *json = nullptr;
    Object3D *object = nullptr;
public:
    JobLoadObject(cJSON *json)
    :
        PendingJob([this](){ process(); }, [this]() { callback(); }),
        json(json)
    {
    }

    void process()
    {
        Logging::Message("[Pools] JobLoadObject process");
        object = JSONSerializerRegistry::instance().deserialize(json);
    }

    void callback()
    {
        //object->setBelongToScene(true);
        //Brakeza::get()->addObject3D(object, object->getName());
        //Logging::Message("[SceneLoaderCreateObject] %s", object->getName().c_str());
    }
};

#endif //BRAKEZA3D_JOBLOADMESH3D_H