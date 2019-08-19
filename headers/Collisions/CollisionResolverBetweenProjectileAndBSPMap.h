#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H


#include "../Physics/Projectile3DBody.h"
#include "../Render/BSPMap.h"
#include "CollisionResolver.h"
#include "../Render/Logging.h"

class CollisionResolverBetweenProjectileAndBSPMap : public CollisionResolver {
public:
    BSPMap *bapMap;
    Projectile3DBody *projectile;

    std::vector<SpriteDirectional3DBody *> *projectiles;
    btDiscreteDynamicsWorld* dynamicsWorld;

    CollisionResolverBetweenProjectileAndBSPMap(Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<SpriteDirectional3DBody *> *projectiles, btDiscreteDynamicsWorld* dynamicsWorld) : CollisionResolver(objA, objB, bspMap)
    {
        this->projectile = getProjectile();
        this->bspMap = getBSPMap();

        this->projectiles = projectiles;
        this->dynamicsWorld = dynamicsWorld;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenProjectileAndBSPMap");
        }

        // Remove projectile for check in stepSimulation
        dynamicsWorld->removeCollisionObject(getProjectile()->getRigidBody());

        // Remove projectile from projectile list
        getProjectile()->setRemoved(true);
    }

    BSPMap *getBSPMap()
    {
        BSPMap *bspMapA = dynamic_cast<BSPMap*> (this->objA);
        if (bspMapA != NULL) {
            return bspMapA;
        }

        BSPMap *bspMapB = dynamic_cast<BSPMap*> (this->objB);
        if (bspMapB != NULL) {
            return bspMapB;
        }
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
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H
