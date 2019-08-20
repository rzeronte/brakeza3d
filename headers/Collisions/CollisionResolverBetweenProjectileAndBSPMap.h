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
    WeaponsManager *weaponManager;

    CollisionResolverBetweenProjectileAndBSPMap(Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<SpriteDirectional3DBody *> *projectiles, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager) : CollisionResolver(objA, objB, bspMap)
    {
        this->projectile = getProjectile();
        this->bspMap = getBSPMap();

        this->projectiles = projectiles;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
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

        Sprite3D *particle = new Sprite3D();
        particle->linkTextureAnimation(weaponManager->getCurrentWeaponType()->getMarkTemplate());
        particle->setAutoRemoveAfterAnimation(true);
        particle->setEnabled(true);
        particle->setPosition(*getProjectile()->getPosition() );
        particle->setTimer(brakeza3D->getTimer());
        particle->setAnimation(0);
        particle->getBillboard()->setDimensions(
            weaponManager->getCurrentWeaponType()->getMarkTemplate()->getBillboard()->width,
            weaponManager->getCurrentWeaponType()->getMarkTemplate()->getBillboard()->height
        );
        brakeza3D->addObject3D(particle, "particles");
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
