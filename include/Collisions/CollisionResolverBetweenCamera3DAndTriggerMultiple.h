
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDTRIGGERMULTIPLE_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDTRIGGERMULTIPLE_H

#include "CollisionResolver.h"
#include "../Render/Logging.h"

class CollisionResolverBetweenCamera3DAndTriggerMultiple : public CollisionResolver {
public:
    Mesh3DGhost *mesh;
    ComponentCamera *camera;
    std::vector<Object3D *> *gameObjects;

    CollisionResolverBetweenCamera3DAndTriggerMultiple(btPersistentManifold *contactManifold, Object3D *objA,
                                                       Object3D *objB, BSPMap *bspMap,
                                                       std::vector<Object3D *> *gameObjects,
                                                       std::vector<Triangle *> &visibleTriangles) : CollisionResolver(
            contactManifold, objA, objB, bspMap, visibleTriangles) {
        this->mesh = this->getMesh3DGhost();
        this->camera = this->getCamera();
        this->gameObjects = gameObjects;
    }

    void dispatch() {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::Log("CollisionResolverBetweenCamera3DAndTriggerMultiple", "Collision");
        }

        int originalEntityIndex = this->mesh->getBspEntityIndex();

        // check for message response
        if (strlen(bspMap->getEntityValue(originalEntityIndex, "message")) > 0) {
            ComponentsManager::get()->getComponentHUD()->writeTextCenter(
                    bspMap->getEntityValue(originalEntityIndex, "message"), false);
        }

    }

    ComponentCamera *getCamera() {
        if (objA->getLabel() == EngineSetup::getInstance()->cameraTriggerNameIdentifier) {
            ComponentCamera *camera = dynamic_cast<ComponentCamera *> (this->objA);
            return camera;
        }

        if (objB->getLabel() == EngineSetup::getInstance()->cameraTriggerNameIdentifier) {
            ComponentCamera *camera = dynamic_cast<ComponentCamera *> (this->objB);
            return camera;
        }
    }

    Mesh3DGhost *getMesh3DGhost() {
        std::string cameraIdentifier = EngineSetup::getInstance()->cameraTriggerNameIdentifier;

        auto *meshA = dynamic_cast<Mesh3DGhost *> (this->objA);
        if (meshA != NULL && strcmp(objA->getLabel().c_str(), cameraIdentifier.c_str()) != 0) {
            return meshA;
        }

        auto *meshB = dynamic_cast<Mesh3DGhost *> (this->objB);
        if (meshB != NULL && strcmp(objB->getLabel().c_str(), cameraIdentifier.c_str()) != 0) {
            return meshB;
        }
    }

};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDTRIGGERMULTIPLE_H
