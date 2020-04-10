
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H


#include "CollisionResolver.h"
#include "../Game/Projectile3DBody.h"
#include "../Game/NPCEnemyBody.h"
#include "../Physics/Sprite3DBody.h"
#include "../Game/NPCEnemyPartBody.h"
#include "../Brakeza3D.h"
#include "../EngineBuffers.h"

class CollisionResolverBetweenProjectileAndNPCEnemy : public CollisionResolver {

public:
    Projectile3DBody *projectile;
    NPCEnemyBody *npcEnemy;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    ComponentWeapons *weaponManager;

    CollisionResolverBetweenProjectileAndNPCEnemy(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, ComponentWeapons *weaponManager, std::vector<Triangle *> &visibleTriangles) : CollisionResolver(contactManifold, objA, objB, bspMap, visibleTriangles)
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

        getNPCEnemy()->takeDamage(weaponManager->getCurrentWeaponType()->getDamage());

        if (getNPCEnemy()->stamina <= 0) {
            ComponentsManager::get()->getComponentGame()->kills++;
            getNPCEnemy()->setDead( true );
            getNPCEnemy()->state = EnemyState::ENEMY_STATE_DIE;

            // Set animation NPC to Dead
            getNPCEnemy()->setAnimation(EngineSetup::getInstance()->SpriteSoldierAnimations::SOLDIER_DEAD);

            bool explosionBody = getNPCEnemy()->isTakeHeavyDamage(weaponManager->getCurrentWeaponType()->getDamage());
            if (explosionBody) {
                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("bodyExplode"), EngineSetup::SoundChannels::SND_EXPLODE_ENEMY, 0);

                for (int i = 0 ; i < 5; i++) {
                    gibsParticles(getNPCEnemy());
                }
            }

            // remove object3D for check in stepSimulation
            dynamicsWorld->removeCollisionObject( (btCollisionObject *) getNPCEnemy()->getRigidBody() );

            // Offset down for draw sprite
            Vertex3D pos = *getNPCEnemy()->getPosition();
            pos.y += 0.85f;
            getNPCEnemy()->setPosition(pos);
            getNPCEnemy()->setBodyEnabled( false);

            makeGoreDecals(-90, 0, 0);

            // Dead sound
            Tools::playMixedSound(EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundEnemyDead"), EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);

            // Remove sprite enemy from world
            //sprite->setRemoved(true);
        } else {
            getNPCEnemy()->state = EnemyState::ENEMY_STATE_INJURIED;
        }

        // Remove projectile for check in stepSimulation
        dynamicsWorld->removeCollisionObject(getProjectile()->getRigidBody());

        // Remove projectile from projectile list
        getProjectile()->setRemoved(true);

        // gore sprite "particles"
        Sprite3D *gore = new Sprite3D();
        gore->linkTextureAnimation(EngineBuffers::getInstance()->bloodTemplates);
        gore->setAutoRemoveAfterAnimation(true);
        gore->setPosition(*getProjectile()->getPosition() );
        gore->setAnimation(Tools::random(0, gore->numAnimations - 1));
        gore->getBillboard()->setDimensions(5, 5);
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

        // BloodHit sound
        Tools::playMixedSound(EngineBuffers::getInstance()->soundPackage->getSoundByLabel("bloodHit" + std::to_string(Tools::random(1, 3))), EngineSetup::SoundChannels::SND_BLOOD_HIT, 0);
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
        gibsBody->makeRigidBody(1.0f, dynamicsWorld);
        Brakeza3D::get()->addObject3D(gibsBody, gibsBody->getLabel());
    }

    void makeGoreDecals(float rotX, float rotY, float rotZ) {
        // decal
        Decal *decal = new Decal();
        decal->setPosition(*getNPCEnemy()->getPosition());
        decal->setupCube(10, 10, 10);
        decal->setRotation(M3::getMatrixRotationForEulerAngles(rotX, rotY, rotZ));
        decal->getSprite()->linkTextureAnimation(EngineBuffers::getInstance()->goreDecalTemplates);
        decal->cube->setPosition(*decal->getPosition());
        decal->cube->update();
        decal->getTriangles(*visibleTriangles, Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera());
        decal->getSprite()->setAnimation(Tools::random(0, 10));
        Brakeza3D::get()->addObject3D(decal, "decal");

    }
};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
