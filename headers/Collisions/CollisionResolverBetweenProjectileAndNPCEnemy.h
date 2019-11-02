
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H


#include "CollisionResolver.h"
#include "../PhysicsGame/Projectile3DBody.h"
#include "../PhysicsGame/NPCEnemyBody.h"
#include "../Physics/Sprite3DBody.h"
#include "../PhysicsGame/NPCEnemyPartBody.h"
#include "../Brakeza3D.h"
#include "../Render/EngineBuffers.h"

class CollisionResolverBetweenProjectileAndNPCEnemy : public CollisionResolver {

public:
    Projectile3DBody *projectile;
    NPCEnemyBody *npcEnemy;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenProjectileAndNPCEnemy(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager, std::vector<Triangle *> &visibleTriangles) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
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

        bool explosionBody = getNPCEnemy()->isTakeHeavyDamage(weaponManager->getCurrentWeaponType()->getDamage());
        getNPCEnemy()->takeDamage(weaponManager->getCurrentWeaponType()->getDamage());

        if (getNPCEnemy()->stamina <= 0) {
            getNPCEnemy()->setDead( true );
            getNPCEnemy()->state = EnemyState::ENEMY_STATE_DIE;

            // Set animation NPC to Dead
            SpriteDirectional3D *sprite = dynamic_cast<SpriteDirectional3D*> (getNPCEnemy());
            if (!explosionBody) {
                sprite->setAnimation(EngineSetup::getInstance()->SpriteSoldierAnimations::SOLDIER_DEAD);
                Logging::getInstance()->Log("Dead soldier");
            } else {
                sprite->setAnimation(EngineSetup::getInstance()->SpriteSoldierAnimations::SOLDIER_EXPLODE);
                Logging::getInstance()->Log("Exploding soldier");

                for (int i = 0 ; i < 5; i++) {
                    gibsParticles(getNPCEnemy());
                }
            }

            // remove object3D for check in stepSimulation
            dynamicsWorld->removeCollisionObject( (btCollisionObject *) getNPCEnemy()->getRigidBody() );

            makeGoreDecals(-90, 0, 0);

            // Dead sound
            Tools::playMixedSound(EngineBuffers::getInstance()->soundEnemyDead);

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
        gore->setAnimation(0);
        gore->getBillboard()->setDimensions(3, 3);
        Brakeza3D::get()->addObject3D(gore, "gore");

        // particle explosion
        /*Sprite3D *particle = new Sprite3D();
        particle->linkTextureAnimation(weaponManager->getCurrentWeaponType()->getMarkTemplate());
        particle->setAutoRemoveAfterAnimation(true);
        particle->setPosition(*getProjectile()->getPosition() );
        particle->setTimer(brakeza3D->getTimer());
        particle->setAnimation(0);
        particle->getBillboard()->setDimensions(
            weaponManager->getCurrentWeaponType()->getMarkTemplate()->getBillboard()->width,
            weaponManager->getCurrentWeaponType()->getMarkTemplate()->getBillboard()->height
        );
        brakeza3D->addObject3D(particle, "particles");*/

        // Mark sound
        Tools::playMixedSound(weaponManager->getCurrentWeaponType()->soundMark);
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
        position.y-=0.5;

        NPCEnemyPartBody *gibsBody = new NPCEnemyPartBody();
        gibsBody->setLabel("gibsBody");
        gibsBody->linkTextureAnimation(EngineBuffers::getInstance()->gibsTemplate);
        gibsBody->setAnimation(Tools::random(0, EngineBuffers::getInstance()->gibsTemplate->numAnimations-1));
        gibsBody->setPosition( position );
        gibsBody->makeRigidBody(1.0f, Brakeza3D::get()->getSceneObjects(), Brakeza3D::get()->getCamera(), dynamicsWorld, enemy);
    }

    void makeGoreDecals(float rotX, float rotY, float rotZ) {
        // decal
        Decal *decal = new Decal();
        decal->setPosition(*getNPCEnemy()->getPosition());
        decal->setupCube(10, 10, 10);
        decal->setRotation(M3::getMatrixRotationForEulerAngles(rotX, rotY, rotZ));
        decal->getSprite()->linkTextureAnimation(EngineBuffers::getInstance()->goreTemplate);
        decal->cube->setPosition(*decal->getPosition());
        decal->cube->update();
        decal->getTriangles(*visibleTriangles, Brakeza3D::get()->getCamera());
        decal->getSprite()->setAnimation(Tools::random(0, 10));
        Brakeza3D::get()->addObject3D(decal, "decal");

    }
};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
