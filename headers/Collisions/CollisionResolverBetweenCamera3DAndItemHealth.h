//
// Created by darkhead on 2/1/20.
//

#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMHEALTH_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMHEALTH_H


#include "CollisionResolver.h"
#include "../Game/ItemHealthBody.h"
#include "../EngineBuffers.h"
#include "../../src/Game/Player.h"

class CollisionResolverBetweenCamera3DAndItemHealth : public CollisionResolver
{
    Camera3D *camera;
    ItemHealthBody *itemHealth;
    Player *player;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

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

    ItemHealthBody* getItemHealthBody()
    {
        auto *itemHealthA = dynamic_cast<ItemHealthBody*> (this->objA);
        if (itemHealthA != nullptr) {
            return itemHealthA;
        }

        auto *itemHealthB = dynamic_cast<ItemHealthBody*> (this->objB);
        if (itemHealthB != nullptr) {
            return itemHealthB;
        }
    }

public:
    CollisionResolverBetweenCamera3DAndItemHealth(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager, std::vector<Triangle *> &visibleTriangles, Player *player) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
    {
        this->camera = getCamera();
        this->itemHealth = getItemHealthBody();

        this->gameObjects   = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
        this->player = player;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenCamera3DAndItemHealth");
        }

        // Skipping if max health
        if (player->getStamina() >= EngineSetup::getInstance()->GAME_PLAYER_STAMINA_INITIAL) return;

        // Remove item for physics engine
        dynamicsWorld->removeCollisionObject( (btCollisionObject *) itemHealth->getRigidBody() );

        // add aid
        player->getAid( itemHealth->getAid() );

        // Remove item health from scene
        this->itemHealth->setRemoved( true );
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("firstAid"), EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDITEMHEALTH_H
