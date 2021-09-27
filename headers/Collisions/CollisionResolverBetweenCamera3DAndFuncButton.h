
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCBUTTON_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCBUTTON_H

#include "CollisionResolver.h"

class CollisionResolverBetweenCamera3DAndFuncButton : public CollisionResolver {
public:
    Mesh3DBody *mesh;
    ComponentCamera *camera;
    std::vector<Object3D *> *gameObjects;

    CollisionResolverBetweenCamera3DAndFuncButton(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB,
                                                  BSPMap *bspMap, std::vector<Object3D *> *gameObjects,
                                                  std::vector<Triangle *> &visibleTriangles) : CollisionResolver(
            contactManifold, objA, objB, bspMap, visibleTriangles) {
        this->mesh = this->getMesh3D();
        this->camera = this->getCamera();
        this->gameObjects = gameObjects;
    }

    void dispatch() {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::Log("CollisionResolverBetweenCamera3DAndFuncButton", "Collision");
        }

        int originalEntityIndex = this->mesh->getBspEntityIndex();
        char *classname = bspMap->getEntityValue(originalEntityIndex, "classname");

        char *targetRemote = bspMap->getEntityValue(originalEntityIndex, "target");
        int targetRemoteEntityId = bspMap->getIndexOfFirstEntityByTargetname(targetRemote);

        auto *originalBody = dynamic_cast<DoorGhost *> (getMesh3D());
        this->moveDoorGhost(originalBody, originalEntityIndex);

        if (targetRemoteEntityId >= 0) {
            char *classnameRemote = bspMap->getEntityValue(targetRemoteEntityId, "classname");
            if (!strcmp(classnameRemote, "func_door")) {
                // Buscamos algún objeto cuya BSPEntity coincida
                for (int k = 0; k < this->gameObjects->size(); k++) {
                    Mesh3D *oRemoteMesh = dynamic_cast<Mesh3D *> ((*this->gameObjects)[k]);
                    if (oRemoteMesh != NULL) {
                        if (oRemoteMesh->getBspEntityIndex() == targetRemoteEntityId) {

                            auto *oRemoteBody = dynamic_cast<DoorGhost *> (oRemoteMesh);
                            this->moveDoorGhost(oRemoteBody, targetRemoteEntityId);

                            if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                                Logging::getInstance()->getInstance()->Log("Moving Door: " + oRemoteBody->getLabel());
                            }
                        }
                    }
                }
            }

            /*if (!strcmp(classnameRemote, "trigger_counter") ) {
                // Si el objeto original era un botón
                if (!strcmp(classname, "func_button")) {
                    Mesh3DBody *oButton = dynamic_cast<Mesh3DBody*> (getDoorGhost());
                    if (oButton->active) {
                        for (int k = 0; k < this->gameObjects->size(); k++) {
                            Mesh3D *oRemoteMesh = dynamic_cast<Mesh3D*> ((*this->gameObjects)[k]);
                            if (oRemoteMesh != NULL) {
                                if (oRemoteMesh->getBspEntityIndex() == targetRemoteEntityId) {

                                    Mesh3DGhost *oRemoteGhost = dynamic_cast<Mesh3DGhost*> (oRemoteMesh);
                                    oRemoteGhost->currentTriggerCounter++;
                                    oButton->active = false;
                                    if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                                        Logging::getInstance()->getInstance()->Log("trigger_counter for BSPEntity: " + std::to_string(targetRemoteEntityId) + "=" + std::to_string(oRemoteGhost->currentTriggerCounter));
                                    }
                                }
                            }
                        }
                    }
                }
            }*/
        }
    }

    ComponentCamera *getCamera() {
        std::string cameraIdentifier = EngineSetup::getInstance()->cameraNameIdentifier;
        if (objA->getLabel() == cameraIdentifier) {
            ComponentCamera *camera = dynamic_cast<ComponentCamera *> (this->objA);
            return camera;
        }

        if (objB->getLabel() == cameraIdentifier) {
            ComponentCamera *camera = dynamic_cast<ComponentCamera *> (this->objB);
            return camera;
        }
    }

    Mesh3DBody *getMesh3D() {
        Mesh3DBody *meshA = dynamic_cast<Mesh3DBody *> (this->objA);
        if (meshA != NULL) {
            return meshA;
        }

        Mesh3DBody *meshB = dynamic_cast<Mesh3DBody *> (this->objB);
        if (meshB != NULL) {
            return meshB;
        }
    }
};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCBUTTON_H
