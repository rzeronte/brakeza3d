
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H


#include "CollisionResolver.h"
#include "../PhysicsGame/Projectile3DBody.h"
#include "../PhysicsGame/NPCEnemyBody.h"
#include "../Physics/Sprite3DBody.h"
#include "../PhysicsGame/NPCEnemyPartBody.h"

class CollisionResolverBetweenProjectileAndNPCEnemy : public CollisionResolver {

public:
    Projectile3DBody *projectile;
    NPCEnemyBody *npcEnemy;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenProjectileAndNPCEnemy(Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager) : CollisionResolver(objA, objB, bspMap)
    {
        this->projectile = this->getProjectile();
        this->npcEnemy   = this->getNPCEnemy();

        this->gameObjects = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenProjectileAndNPCEnemy");
        }

        Enemy *oSpriteDirectionalEnemyB = dynamic_cast<Enemy*> (getNPCEnemy());

        bool explosionBody = getNPCEnemy()->isHeavyDamage(weaponManager->getCurrentWeaponType()->getDamage());
        getNPCEnemy()->damage(weaponManager->getCurrentWeaponType()->getDamage());

        if (getNPCEnemy()->stamina < 0) {
            // Set animation NPC to Dead
            SpriteDirectional3D *sprite = dynamic_cast<SpriteDirectional3D*> (getNPCEnemy());
            if (!explosionBody) {
                sprite->setAnimation(EngineSetup::getInstance()->SpriteDoom2SoldierAnimations::SOLDIER_DEAD);
                Logging::getInstance()->Log("Dead soldier");
            } else {
                sprite->setAnimation(EngineSetup::getInstance()->SpriteDoom2SoldierAnimations::SOLDIER_EXPLODE);
                Logging::getInstance()->Log("Exploding soldier");

                for (int i = 0 ; i < 4; i++) {
                    gibsParticles(getNPCEnemy());
                }
            }

            // remove object3D for check in stepSimulation
            dynamicsWorld->removeCollisionObject( (btCollisionObject *) getNPCEnemy()->getRigidBody() );

            makeGoreDecals(-90, 0, 0);

            // Remove sprite enemy from world
            //sprite->setRemoved(true);
        }

        // Remove projectile for check in stepSimulation
        dynamicsWorld->removeCollisionObject(getProjectile()->getRigidBody());

        // Remove projectile from projectile list
        getProjectile()->setRemoved(true);

        // gore sprite "particles"
        Sprite3D *gore = new Sprite3D();
        gore->linkTextureAnimation(EngineBuffers::getInstance()->goreTemplate);
        gore->setAutoRemoveAfterAnimation(true);
        gore->setPosition(*getProjectile()->getPosition() );
        gore->setTimer(brakeza3D->getTimer());
        gore->setAnimation(0);
        gore->getBillboard()->setDimensions(3, 3);
        brakeza3D->addObject3D(gore, "gore");

        // particle explosion
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

    void gibsParticles(NPCEnemyBody *enemy)
    {
        Vertex3D position = *enemy->getPosition();
        position.y-=1.5;

        NPCEnemyPartBody *doomFaceBody = new NPCEnemyPartBody();
        doomFaceBody->setLabel("NPCPartBody");
        doomFaceBody->setEnabled(true);
        doomFaceBody->setPosition( position );
        doomFaceBody->setTimer(brakeza3D->getTimer());
        doomFaceBody->addAnimation("doom_face/face", 3, 10);
        doomFaceBody->setAnimation(EngineSetup::getInstance()->SpriteGuyAnimations::NORMAL);
        doomFaceBody->getBillboard()->setDimensions(1, 1);
        doomFaceBody->makeRigidBody(1.0f, brakeza3D->gameObjects, brakeza3D->camera, dynamicsWorld, enemy);
    }

    void makeGoreDecals(float rotX, float rotY, float rotZ) {

    }
};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
