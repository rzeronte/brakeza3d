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
            LOG_MESSAGE("[Mesh3D] SetupRigidBodyCollider waiting...");
        }

        LOG_MESSAGE("[Mesh3D] SetupRigidBodyCollider in object '%s' - CollisionShape: %d", mesh->getName().c_str(), shape);
        mesh->RemoveCollisionObject();

        mesh->setCollisionShape(shape);
        mesh->setCollisionMode(BODY);
        if (mesh->getCollisionShape() == SIMPLE_SHAPE || mesh->getCollisionShape() == CAPSULE_SHAPE) {
            mesh->MakeSimpleRigidBody(
                mesh->getMass(),
                Components::get()->Collisions()->getDynamicsWorld(),
                btBroadphaseProxy::DefaultFilter,
                btBroadphaseProxy::DefaultFilter
            );
        }

        if (mesh->getCollisionShape() == TRIANGLE_MESH_SHAPE) {
            if (mesh->isColliderStatic()) {
                mesh->makeRigidBodyFromTriangleMesh(
                    mesh->getMass(),
                    Components::get()->Collisions()->getDynamicsWorld(),
                    Config::collisionGroups::AllFilter,
                    Config::collisionGroups::AllFilter
                );
            } else {
                mesh->makeRigidBodyFromTriangleMeshFromConvexHull(
                    mesh->getMass(),
                    Components::get()->Collisions()->getDynamicsWorld(),
                    Config::collisionGroups::AllFilter,
                    Config::collisionGroups::AllFilter
                );
            }
        }

        LOG_MESSAGE("[ThreadJobMakeRigidBody] Process END");
    }

    void fnCallback()
    {
        LOG_MESSAGE("[ThreadJobMakeRigidBody] Callback END");
    }
};
#endif //BRAKEZA3D_THREADJOBMAKERIGIDBODY_H