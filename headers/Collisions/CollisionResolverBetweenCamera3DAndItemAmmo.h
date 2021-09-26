//
// Created by darkhead on 2/1/20.
//

#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMAMMO_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMAMMO_H


#include "CollisionResolver.h"
#include "../Game/ItemAmmoGhost.h"
#include "../EngineBuffers.h"
#include "../../src/Game/Player.h"

class CollisionResolverBetweenCamera3DAndItemAmmo : public CollisionResolver {
public:
    ComponentCamera *camera;
    ItemAmmoGhost *itemAmmo;
    Player *player;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld *dynamicsWorld;
    ComponentWeapons *weaponManager;

    CollisionResolverBetweenCamera3DAndItemAmmo(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB,
                                                BSPMap *bspMap, std::vector<Object3D *> *gameObjects,
                                                btDiscreteDynamicsWorld *dynamicsWorld, ComponentWeapons *weaponManager,
                                                std::vector<Triangle *> &visibleTriangles, Player *player)
            : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles) {
        this->camera = getCamera();
        this->itemAmmo = getItemAmmoBody();

        this->gameObjects = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
        this->player = player;
    }

    void dispatch() {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenCamera3DAndItemAmmo");
        }

        AmmoType *ammoType = weaponManager->getAmmoTypeByClassname(itemAmmo->getAmmoTypeClassname());

        if (ammoType != nullptr) {

            // Remove item for physics engine
            dynamicsWorld->removeCollisionObject((btCollisionObject *) itemAmmo->ghostObject);
            ammoType->setAmount(ammoType->getAmount() + itemAmmo->getAmount());

            this->itemAmmo->setRemoved(true);
            Tools::playMixedSound(EngineBuffers::getInstance()->soundPackage->getSoundByLabel("getAmmo"),
                                  EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
        }
    }

    ComponentCamera *getCamera() {
        if (objA->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            ComponentCamera *camera = dynamic_cast<ComponentCamera *> (this->objA);
            return camera;
        }

        if (objB->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            ComponentCamera *camera = dynamic_cast<ComponentCamera *> (this->objB);
            return camera;
        }
    }

    ItemAmmoGhost *getItemAmmoBody() {
        auto *itemTmpA = dynamic_cast<ItemAmmoGhost *> (this->objA);
        if (itemTmpA != nullptr) {
            return itemTmpA;
        }

        auto *itemTmpB = dynamic_cast<ItemAmmoGhost *> (this->objB);
        if (itemTmpB != nullptr) {
            return itemTmpB;
        }
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMAMMO_H
