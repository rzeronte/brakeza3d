//
// Created by darkhead on 2/1/20.
//

#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMAMMO_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMAMMO_H


#include "CollisionResolver.h"
#include "../Game/ItemAmmoBody.h"
#include "../EngineBuffers.h"
#include "../../src/Game/Player.h"

class CollisionResolverBetweenCamera3DAndItemAmmo : public CollisionResolver {
public:
    Camera3D *camera;
    ItemAmmoBody *itemAmmo;
    Player *player;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenCamera3DAndItemAmmo(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager, std::vector<Triangle *> &visibleTriangles, Player *player) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
    {
        this->camera = getCamera();
        this->itemAmmo = getItemAmmoBody();

        this->gameObjects   = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
        this->player = player;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenCamera3DAndItemAmmo");
        }

        AmmoType *ammoType = weaponManager->getAmmoTypeByClassname( itemAmmo->getAmmoTypeClassname() );

        if (ammoType != nullptr) {

            // Remove item for physics engine
            dynamicsWorld->removeCollisionObject( (btCollisionObject *) itemAmmo->getRigidBody() );
            ammoType->setAmount( ammoType->getAmount() + itemAmmo->getAmount() );

            this->itemAmmo->setRemoved( true );
            Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("getAmmo"), EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
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

    ItemAmmoBody* getItemAmmoBody()
    {
        auto *itemTmpA = dynamic_cast<ItemAmmoBody*> (this->objA);
        if (itemTmpA != nullptr) {
            return itemTmpA;
        }

        auto *itemTmpB = dynamic_cast<ItemAmmoBody*> (this->objB);
        if (itemTmpB != nullptr) {
            return itemTmpB;
        }
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMAMMO_H
