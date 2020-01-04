//
// Created by darkhead on 1/1/20.
//

#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMWEAPON_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMWEAPON_H


#include "CollisionResolver.h"
#include "../Game/ItemWeaponBody.h"

class CollisionResolverBetweenCamera3DAndItemWeapon : public CollisionResolver {
public:
    Camera3D *camera;
    ItemWeaponBody *itemWeaponBody;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenCamera3DAndItemWeapon(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager, std::vector<Triangle *> &visibleTriangles) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
    {
        this->camera = getCamera();
        this->itemWeaponBody = getItemWeaponBody();

        this->gameObjects   = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenCamera3DAndItemWeapon");
        }

        WeaponType * wt = weaponManager->getWeaponTypeByClassname( itemWeaponBody->getWeaponClassname() );

        if (wt != nullptr) {
            // Remove item for physics engine
            dynamicsWorld->removeCollisionObject( (btCollisionObject *) itemWeaponBody->getRigidBody() );

            // Activate weapon
            wt->setAvailable( true );

            // Remove itemweapon body from scene
            this->itemWeaponBody->setRemoved( true );
            Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("getWeapon"), EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);

            if (EngineSetup::getInstance()->WEAPON_AUTOSWICH) {
                weaponManager->currentWeaponIndex = wt->getIndex();
            }
        }
    }

    Camera3D* getCamera()
    {
        if (objA->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            Camera3D *camera = dynamic_cast<Camera3D*> (this->objA);
            return camera;
        }

        if (objB->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            Camera3D *camera = dynamic_cast<Camera3D*> (this->objB);
            return camera;
        }
    }

    ItemWeaponBody* getItemWeaponBody()
    {
        auto *itemWeaponA = dynamic_cast<ItemWeaponBody*> (this->objA);
        if (itemWeaponA != NULL) {
            return itemWeaponA;
        }

        auto *itemWeaponB = dynamic_cast<ItemWeaponBody*> (this->objB);
        if (itemWeaponB != NULL) {
            return itemWeaponB;
        }
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMWEAPON_H
