
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H


#include "CollisionResolver.h"
#include "../Physics/Projectile3DBody.h"
#include "../Physics/NPCEnemyBody.h"

class CollisionResolverBetweenProjectileAndNPCEnemy : public CollisionResolver {

public:
    Projectile3DBody *projectile;
    NPCEnemyBody *npcEnemy;

    std::vector<SpriteDirectional3DBody *> *projectiles;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenProjectileAndNPCEnemy(Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<SpriteDirectional3DBody *> *projectilePhysics, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager) : CollisionResolver(objA, objB, bspMap)
    {
        this->projectile = this->getProjectile();
        this->npcEnemy   = this->getNPCEnemy();

        this->projectiles = projectilePhysics;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenProjectileAndNPCEnemy");
        }

        Enemy *oSpriteDirectionalEnemyB = dynamic_cast<Enemy*> (getNPCEnemy());

        getNPCEnemy()->setDamage( 5 );

        if (getNPCEnemy()->stamina < 0) {
            // Set animation NPC to Dead
            SpriteDirectional3D *sprite = dynamic_cast<SpriteDirectional3D*> (getNPCEnemy());
            sprite->setAnimation(EngineSetup::getInstance()->SpriteDoom2SoldierAnimations::SOLDIER_DEAD);

            // remove object3D for check in stepSimulation
            dynamicsWorld->removeCollisionObject( (btCollisionObject *) getNPCEnemy()->getRigidBody() );
            sprite->setRemoved(true);
        }

        // Remove projectile for check in stepSimulation
        dynamicsWorld->removeCollisionObject(getProjectile()->getRigidBody());

        // Remove projectile from projectile list
        getProjectile()->setRemoved(true);

        Sprite3D *gore = new Sprite3D();
        gore->linkTextureAnimation(EngineBuffers::getInstance()->goreTemplate);
        gore->setAutoRemoveAfterAnimation(true);
        gore->setPosition(*getProjectile()->getPosition() );
        gore->setTimer(brakeza3D->getTimer());
        gore->setAnimation(0);
        gore->getBillboard()->setDimensions(3, 3);
        brakeza3D->addObject3D(gore, "gore");
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

    NPCEnemyBody *getNPCEnemy()
    {
        NPCEnemyBody *enemyA = dynamic_cast<NPCEnemyBody*> (this->objA);
        if (enemyA != NULL) {
            return enemyA;
        }

        NPCEnemyBody *enemyB = dynamic_cast<NPCEnemyBody*> (this->objB);
        if (enemyB != NULL) {
            return enemyB;
        }
    }

};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
