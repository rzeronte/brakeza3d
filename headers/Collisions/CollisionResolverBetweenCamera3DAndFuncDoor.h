
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCDOOR_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCDOOR_H

#include "CollisionResolver.h"
#include "../Render/Logging.h"

class CollisionResolverBetweenCamera3DAndFuncDoor : public CollisionResolver {
public:
    DoorGhost *mesh;
    ComponentCamera *camera;
    std::vector<Object3D*> *gameObjects;

    CollisionResolverBetweenCamera3DAndFuncDoor(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D*> *gameObjects, std::vector<Triangle *> &visibleTriangles) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
    {
        this->mesh   = this->getDoorGhost();
        this->camera = this->getCamera();
        this->gameObjects = gameObjects;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenCamera3DAndFuncDoor");
        }

        int originalEntityIndex = this->mesh->getBspEntityIndex();
        char *currentTargetName = bspMap->getEntityValue(originalEntityIndex, "targetname");

        if (!bspMap->hasEntityAttribute(originalEntityIndex, "targetname")) {
            // No tiene targetname
            auto *originalBody = dynamic_cast<DoorGhost*> (getDoorGhost());

            if (originalBody != nullptr) {
                if (originalBody->isMoving()) return;

                this->moveDoorGhost(originalBody, originalEntityIndex);
                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("openDoor"), EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
                if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                    Logging::getInstance()->getInstance()->Log("moveDoorGhost: " + originalBody->getLabel());
                }
            }
        } else {
            int targetRemoteEntityId = bspMap->getIndexOfFirstEntityByTarget( currentTargetName );
            char *classnameRemote = bspMap->getEntityValue(targetRemoteEntityId, "classname");

            if ( !strcmp(classnameRemote, "trigger_counter") ) {
                for (int k = 0; k < this->gameObjects->size(); k++) {
                    Mesh3D *oRemoteMesh = dynamic_cast<Mesh3D*> ((*this->gameObjects)[k]);
                    if (oRemoteMesh != nullptr) {
                        if (oRemoteMesh->getBspEntityIndex() == targetRemoteEntityId) {
                            Mesh3DGhost *oRemoteGhost = dynamic_cast<Mesh3DGhost*> (oRemoteMesh);
                            int currentCounter = oRemoteGhost->currentTriggerCounter;

                            char *countValue = bspMap->getEntityValue(targetRemoteEntityId, "count");
                            int countValueInt = atoi( std::string(countValue).c_str() );

                            if (countValueInt == currentCounter) {
                                auto *originalBody = dynamic_cast<DoorGhost*> (getDoorGhost());

                                this->moveDoorGhost(originalBody, originalEntityIndex);
                            } else {
                                if (strlen(bspMap->getEntityValue(originalEntityIndex, "message")) > 0) {
                                    //Tools::writeTextCenter(Engine::renderer, Engine::font, Color::white(), std::string(bspMap->getEntityValue(originalEntityIndex, "message")) );
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    ComponentCamera* getCamera()
    {
        if (objA->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            ComponentCamera *camera = dynamic_cast<ComponentCamera*> (this->objA);
            return camera;
        }

        if (objB->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            ComponentCamera *camera = dynamic_cast<ComponentCamera*> (this->objB);
            return camera;
        }
    }

    DoorGhost* getDoorGhost()
    {
        DoorGhost *meshA = dynamic_cast<DoorGhost*> (this->objA);
        if (meshA != NULL) {
            return meshA;
        }

        DoorGhost *meshB = dynamic_cast<DoorGhost*> (this->objB);
        if (meshB != NULL) {
            return meshB;
        }
    }

};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCDOOR_H
