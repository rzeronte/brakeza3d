
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDCAMERA3D_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDCAMERA3D_H


#include "CollisionResolver.h"
#include "../PhysicsGame/Projectile3DBody.h"
#include "../PhysicsGame/NPCEnemyBody.h"
#include "../Physics/Sprite3DBody.h"
#include "../PhysicsGame/NPCEnemyPartBody.h"

class CollisionResolverBetweenProjectileAndCamera3D : public CollisionResolver {

public:
    Projectile3DBody *projectile;
    Camera3D *camera;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenProjectileAndCamera3D(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager, std::vector<Triangle *> &visibleTriangles) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
    {
        this->projectile = this->getProjectile();
        this->camera     = this->getCamera();

        this->gameObjects   = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenProjectileAndCamera3D");
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

    Camera3D *getCamera()
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

};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
