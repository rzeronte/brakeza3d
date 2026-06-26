//
// Created by Eduardo on 21/12/2025.
//

#ifndef BRAKEZA3D_THREADJOBMAKERIGIDBODY_H
#define BRAKEZA3D_THREADJOBMAKERIGIDBODY_H

#include "ThreadJobBase.h"
#include "../3D/Mesh3D.h"
#include "../Components/Components.h"
#include "../Misc/Logging.h"

class ThreadJobMakeRigidBody : public ThreadJobBase
{
    Mesh3D* mesh;
    CollisionShape shape;
    btRigidBody* builtBody = nullptr;
public:
    ThreadJobMakeRigidBody(Mesh3D* mesh, CollisionShape modeShape)
    :
        mesh(mesh),
        shape(modeShape)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        while (!mesh->isLoaded()) {
            LOG_MESSAGE("[ThreadJobMakeRigidBody] waiting for mesh load...");
        }

        LOG_MESSAGE("[ThreadJobMakeRigidBody] Building body for '%s' - CollisionShape: %d", mesh->getName().c_str(), shape);

        float mass = mesh->getMass();

        if (shape == SIMPLE_SHAPE || shape == CAPSULE_SHAPE) {
            mesh->setCollisionShape(shape);
            builtBody = mesh->BuildSimpleRigidBodyOnly(mass);
        }

        if (shape == TRIANGLE_MESH_SHAPE) {
            mesh->setCollisionShape(shape);
            if (mesh->isColliderStatic()) {
                builtBody = mesh->BuildRigidBodyFromTriangleMeshOnly(mass);
            } else {
                builtBody = mesh->BuildRigidBodyFromConvexHullOnly(mass);
            }
        }

        LOG_MESSAGE("[ThreadJobMakeRigidBody] Build done");
    }

    void fnCallback()
    {
        if (builtBody == nullptr) {
            LOG_ERROR("[ThreadJobMakeRigidBody] builtBody is null, skipping addRigidBody");
            return;
        }

        auto* world = Components::get()->Collisions()->getDynamicsWorld();

        mesh->RemoveCollisionObject();
        mesh->setCollisionMode(BODY);
        mesh->setBody(builtBody);

        if (shape == SIMPLE_SHAPE || shape == CAPSULE_SHAPE) {
            world->addRigidBody(builtBody, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::DefaultFilter);
        } else {
            world->addRigidBody(builtBody, Config::collisionGroups::AllFilter, Config::collisionGroups::AllFilter);
        }

        LOG_MESSAGE("[ThreadJobMakeRigidBody] Registered in dynamics world");
    }
};
#endif //BRAKEZA3D_THREADJOBMAKERIGIDBODY_H
