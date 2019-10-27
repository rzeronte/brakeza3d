
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDCAMERA3D_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDCAMERA3D_H


#include "CollisionResolver.h"
#include "../PhysicsGame/Projectile3DBody.h"
#include "../PhysicsGame/NPCEnemyBody.h"
#include "../Physics/Sprite3DBody.h"
#include "../PhysicsGame/NPCEnemyPartBody.h"
#include "../../src/Objects/Player3D.h"

class CollisionResolverBetweenProjectileAndPlayer : public CollisionResolver {

public:
    Projectile3DBody *projectile;
    Player3D *player;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenProjectileAndPlayer(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager, std::vector<Triangle *> &visibleTriangles) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
    {
        this->projectile = this->getProjectile();
        this->player     = this->getPlayer3D();

        this->gameObjects   = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenProjectileAndPlayer");
        }

        // Remove projectile for check in stepSimulation
        dynamicsWorld->removeCollisionObject(getProjectile()->getRigidBody());

        // Remove projectile from projectile list
        getProjectile()->setRemoved(true);

    }

    Projectile3DBody *getProjectile()
    {
        Projectile3DBody *projectileA = dynamic_cast<Projectile3DBody*> (this->objA);
        if (projectileA != NULL) {
            return projectileA;
        }

        Projectile3DBody *projectileB = dynamic_cast<Projectile3DBody*> (this->objB);
        if (projectileB != NULL) {
            return projectileB;
        }
    }

    Player3D *getPlayer3D()
    {
        if (objA->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            Player3D *camera = dynamic_cast<Player3D*> (this->objA);
            return camera;
        }

        if (objB->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            Player3D *camera = dynamic_cast<Player3D*> (this->objB);
            return camera;
        }
    }

};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
