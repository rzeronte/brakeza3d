
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDCAMERA3D_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDCAMERA3D_H


#include "CollisionResolver.h"
#include "../Game/Projectile3DBody.h"
#include "../Game/NPCEnemyBody.h"
#include "../Physics/Sprite3DBody.h"
#include "../Game/NPCEnemyPartBody.h"
#include "../../src/Game/Player.h"

class CollisionResolverBetweenProjectileAndPlayer : public CollisionResolver {

public:
    Projectile3DBody *projectile;
    Player *player;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld *dynamicsWorld;
    ComponentWeapons *weaponManager;

    CollisionResolverBetweenProjectileAndPlayer(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB,
                                                BSPMap *bspMap, std::vector<Object3D *> *gameObjects,
                                                btDiscreteDynamicsWorld *dynamicsWorld, ComponentWeapons *weaponManager,
                                                std::vector<Triangle *> &visibleTriangles, Player *player)
            : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles) {
        this->projectile = this->getProjectile();
        this->player = player;

        this->gameObjects = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch() {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenProjectileAndPlayer");
        }

        // Remove projectile for check in stepSimulation
        dynamicsWorld->removeCollisionObject(getProjectile()->getRigidBody());

        // Remove projectile from projectile list
        getProjectile()->setRemoved(true);

        player->takeDamage(1);
    }

    Projectile3DBody *getProjectile() {
        Projectile3DBody *projectileA = dynamic_cast<Projectile3DBody *> (this->objA);
        if (projectileA != NULL) {
            return projectileA;
        }

        Projectile3DBody *projectileB = dynamic_cast<Projectile3DBody *> (this->objB);
        if (projectileB != NULL) {
            return projectileB;
        }
    }

};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
