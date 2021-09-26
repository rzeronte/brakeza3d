//
// Created by darkhead on 1/1/20.
//

#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMWEAPON_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMWEAPON_H


#include "CollisionResolver.h"
#include "../Game/ItemWeaponGhost.h"

class CollisionResolverBetweenCamera3DAndItemWeapon : public CollisionResolver {
public:
    ComponentCamera *camera;
    ItemWeaponGhost *itemWeaponBody;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld *dynamicsWorld;
    ComponentWeapons *weaponManager;

    CollisionResolverBetweenCamera3DAndItemWeapon(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB,
                                                  BSPMap *bspMap, std::vector<Object3D *> *gameObjects,
                                                  btDiscreteDynamicsWorld *dynamicsWorld,
                                                  ComponentWeapons *weaponManager,
                                                  std::vector<Triangle *> &visibleTriangles) : CollisionResolver(
            contactManifold, objA, objB, bspMap, visibleTriangles) {
        this->camera = getCamera();
        this->itemWeaponBody = getItemWeaponBody();

        this->gameObjects = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch() {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenCamera3DAndItemWeapon");
        }

        WeaponType *wt = weaponManager->getWeaponTypeByClassname(itemWeaponBody->getWeaponClassname());

        if (wt != nullptr) {
            // Remove item for physics engine
            dynamicsWorld->removeCollisionObject((btCollisionObject *) itemWeaponBody->ghostObject);

            // Activate weapon
            wt->setAvailable(true);

            // Remove itemweapon body from scene
            this->itemWeaponBody->setRemoved(true);
            Tools::playMixedSound(EngineBuffers::getInstance()->soundPackage->getSoundByLabel("getWeapon"),
                                  EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);

            if (EngineSetup::getInstance()->WEAPON_AUTOSWICH) {
                weaponManager->currentWeaponIndex = wt->getIndex();
            }
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

    ItemWeaponGhost *getItemWeaponBody() {
        auto *itemWeaponA = dynamic_cast<ItemWeaponGhost *> (this->objA);
        if (itemWeaponA != NULL) {
            return itemWeaponA;
        }

        auto *itemWeaponB = dynamic_cast<ItemWeaponGhost *> (this->objB);
        if (itemWeaponB != NULL) {
            return itemWeaponB;
        }
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMWEAPON_H
