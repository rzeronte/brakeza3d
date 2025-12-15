//
// Created by Eduardo on 15/12/2025.
//

#ifndef BRAKEZA3D_OBJECTFACTORY_H
#define BRAKEZA3D_OBJECTFACTORY_H
#include "../Brakeza.h"
#include "../3D/Mesh3D.h"
#include "../Render/JSONSerializerRegistry.h"
#include "../Serializers/Mesh3DSerializer.h"
#include "../Threads/ThreadJobLoadMesh3D.h"

class ObjectFactory {

    public:
    static void CreateMesh3D(const std::string &file, const Vertex3D &position)
    {
        auto *o = new Mesh3D(file);
        o->setName(Brakeza::UniqueObjectLabel("Mesh3D"));
        o->setPosition(position);

        auto json = JSONSerializerRegistry::instance().serialize(o);
        Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(std::make_shared<ThreadJobLoadMesh3D>(o, json));
    }
};

#endif //BRAKEZA3D_OBJECTFACTORY_H