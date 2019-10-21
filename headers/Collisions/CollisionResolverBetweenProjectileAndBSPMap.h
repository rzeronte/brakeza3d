#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H


#include "../PhysicsGame/Projectile3DBody.h"
#include "../Render/BSPMap.h"
#include "CollisionResolver.h"
#include "../Render/Logging.h"

class CollisionResolverBetweenProjectileAndBSPMap : public CollisionResolver {
public:
    BSPMap *bapMap;
    Projectile3DBody *projectile;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenProjectileAndBSPMap(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager) : CollisionResolver(contactManifold, objA, objB, bspMap)
    {
        this->projectile = getProjectile();
        this->bspMap = getBSPMap();

        this->gameObjects = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenProjectileAndBSPMap");
            Logging::getInstance()->Log("contactManifold: numContacts" + std::to_string(contactManifold->getNumContacts()));
        }

        // Remove projectile for check in stepSimulation
        dynamicsWorld->removeCollisionObject(getProjectile()->getRigidBody());

        // Remove projectile from projectile list
        getProjectile()->setRemoved(true);

        Sprite3D *particle = new Sprite3D();
        particle->linkTextureAnimation(weaponManager->getCurrentWeaponType()->getMarkTemplate());
        particle->setAutoRemoveAfterAnimation(true);
        particle->setPosition(*getProjectile()->getPosition() );
        particle->setTimer(brakeza3D->getTimer());
        particle->setAnimation(0);
        particle->getBillboard()->setDimensions(
            weaponManager->getCurrentWeaponType()->getMarkTemplate()->getBillboard()->width,
            weaponManager->getCurrentWeaponType()->getMarkTemplate()->getBillboard()->height
        );
        brakeza3D->addObject3D(particle, "particles");

        Tools::playMixedSound(weaponManager->getCurrentWeaponType()->soundMark);

        //makeGoreDecals();
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

    void makeGoreDecals()
    {
        Decal *decal = new Decal();
        decal->setPosition(*getProjectile()->getPosition());
        decal->setupCube(10, 10, 10);

        btVector3 linearVelocity;
        btVector3 ptA, ptB;
        for (int x = 0; x < contactManifold->getNumContacts(); x++) {
            btManifoldPoint manifoldPoint = contactManifold->getContactPoint(x);
            linearVelocity = manifoldPoint.m_normalWorldOnB;
        }

        //linearVelocity = projectile->getRigidBody()->getLinearVelocity().normalized();
        Vertex3D direction = Vertex3D(linearVelocity.x(), linearVelocity.y(), linearVelocity.z());
        direction = direction.getInverse();

        M3 rotDecal = M3::getFromVectors(direction.getNormalize(), EngineSetup::getInstance()->up);
        decal->setRotation(rotDecal.getTranspose());

        decal->getSprite()->linkTextureAnimation(EngineBuffers::getInstance()->goreTemplate);
        decal->getSprite()->setAnimation(Tools::random(0, 10));
        decal->cube->setPosition(*decal->getPosition());
        decal->cube->update();
        decal->getTriangles(brakeza3D->visibleTriangles, brakeza3D->numVisibleTriangles, brakeza3D->camera);
        brakeza3D->addObject3D(decal, "decal");
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H
