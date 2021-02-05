
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDTRIGGERTELEPORT_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDTRIGGERTELEPORT_H

#include "CollisionResolver.h"
#include "../Render/Logging.h"

class CollisionResolverBetweenCamera3DAndTriggerTeleport : public CollisionResolver {
public:
    Mesh3DGhost *mesh;
    std::vector<Object3D*> *gameObjects;

    CollisionResolverBetweenCamera3DAndTriggerTeleport(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D*> *gameObjects, std::vector<Triangle *> &visibleTriangles) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
    {
        this->mesh   = this->getMesh3DGhost();
        this->gameObjects = gameObjects;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenCamera3DAndTriggerTeleport");
        }

        int originalEntityId = this->mesh->getBspEntityIndex();
        int targetEntityId = bspMap->getIndexOfFirstEntityByTargetname(bspMap->getEntityValue(originalEntityId, "target") );

        if (targetEntityId < 0) {
            return;
        }

        if (this->bspMap->hasEntityAttribute(targetEntityId, "origin")) {
            char *value = bspMap->getEntityValue(targetEntityId, "origin");
            char *angle = bspMap->getEntityValue(targetEntityId, "angle");
            Vertex3D teleportPos = bspMap->parsePositionFromEntityAttribute(value);

            int angleInt = atoi( std::string(angle).c_str() );
            Camera3D *camera = ComponentsManager::get()->getComponentCamera()->getCamera();
            camera->yaw   = 90-angleInt;
            camera->pitch = 0;
            camera->roll  = 0;

            camera->setPosition( teleportPos + Vertex3D(0, 0, 32));

            ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->teleportingCounter.setEnabled(true);
            ComponentsManager::get()->getComponentBSP()->getBSPCollider()->resetPlayerModelData();
        }
    }

    Mesh3DGhost* getMesh3DGhost()
    {
        std::string cameraIdentifier = EngineSetup::getInstance()->cameraTriggerNameIdentifier;

        auto *meshA = dynamic_cast<Mesh3DGhost*> (this->objA);
        if (meshA != NULL && strcmp(objA->getLabel().c_str(), cameraIdentifier.c_str()) != 0) {
            return meshA;
        }

        auto *meshB = dynamic_cast<Mesh3DGhost*> (this->objB);
        if (meshB != NULL && strcmp(objB->getLabel().c_str(), cameraIdentifier.c_str()) != 0) {
            return meshB;
        }
    }
};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDTRIGGERTELEPORT_H
