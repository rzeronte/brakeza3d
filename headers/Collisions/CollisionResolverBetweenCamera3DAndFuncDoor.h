
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCDOOR_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCDOOR_H

#include "CollisionResolver.h"
#include "../Render/Logging.h"

class CollisionResolverBetweenCamera3DAndFuncDoor : public CollisionResolver {
public:
    Mesh3DBody *mesh;
    ComponentCamera *camera;
    std::vector<Object3D*> *gameObjects;

    CollisionResolverBetweenCamera3DAndFuncDoor(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D*> *gameObjects, std::vector<Triangle *> &visibleTriangles) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
    {
        this->mesh   = this->getMesh3D();
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
            Mesh3DBody *originalBody = dynamic_cast<Mesh3DBody*> (getMesh3D());

            if (originalBody != nullptr) {
                if (originalBody->isMoving()) return;

                this->moveMesh3DBody(originalBody, originalEntityIndex);
                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("openDoor"), EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
                if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
                    Logging::getInstance()->getInstance()->Log("moveMesh3DBody: " + originalBody->getLabel());
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
                                Mesh3DBody *originalBody = dynamic_cast<Mesh3DBody*> (getMesh3D());

                                this->moveMesh3DBody(originalBody, originalEntityIndex);
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

    Mesh3DBody* getMesh3D()
    {
        Mesh3DBody *meshA = dynamic_cast<Mesh3DBody*> (this->objA);
        if (meshA != NULL) {
            return meshA;
        }

        Mesh3DBody *meshB = dynamic_cast<Mesh3DBody*> (this->objB);
        if (meshB != NULL) {
            return meshB;
        }
    }

};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCDOOR_H
