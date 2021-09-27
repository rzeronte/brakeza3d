#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H


#include "../Game/Projectile3DBody.h"
#include "../Render/BSPMap.h"
#include "CollisionResolver.h"
#include "../Render/Logging.h"
#include "../Game/NPCEnemyPartBody.h"
#include "../Brakeza3D.h"

class CollisionResolverBetweenProjectileAndBSPMap : public CollisionResolver {
public:
    Projectile3DBody *projectile;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld *dynamicsWorld;
    ComponentWeapons *weaponManager;

    CollisionResolverBetweenProjectileAndBSPMap(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB,
                                                BSPMap *bspMap, std::vector<Object3D *> *gameObjects,
                                                btDiscreteDynamicsWorld *dynamicsWorld, ComponentWeapons *weaponManager,
                                                std::vector<Triangle *> &visibleTriangles) : CollisionResolver(
            contactManifold, objA, objB, bspMap, visibleTriangles) {
        this->projectile = getProjectile();

        this->gameObjects = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch() {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::Log("CollisionResolverBetweenProjectileAndBSPMap", "Collision");
        }

        // Remove projectile for check in stepSimulation
        dynamicsWorld->removeCollisionObject(getProjectile()->getRigidBody());

        // Remove projectile from projectile list
        getProjectile()->setRemoved(true);

        // Enemies dont make "particles"
        if (!getProjectile()->isFromEnemy()) {

            // make damage radius
            if (projectile->getDamageRadius() > 0) {
                makeDamageRadius();
            }
        }
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

    void makeGoreDecals() {
        Decal *decal = new Decal();
        decal->setPosition(getProjectile()->getPosition());
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

        decal->getSprite()->linkTextureAnimation(EngineBuffers::getInstance()->goreDecalTemplates);
        decal->getSprite()->setAnimation(Tools::random(0, 10));
        decal->cube->setPosition(decal->getPosition());
        decal->cube->updateGeometry();
        decal->getTriangles(*visibleTriangles,
                            Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera());
        Brakeza3D::get()->addObject3D(decal, "decal");
    }

    void makeDamageRadius() {
        Logging::Log("makeDamageRadius", "Collisions");
        std::vector<Object3D *>::iterator itObject3D;
        int cont = 0;
        for (itObject3D = Brakeza3D::get()->getSceneObjects().begin();
             itObject3D != Brakeza3D::get()->getSceneObjects().end(); itObject3D++) {
            Object3D *object = *(itObject3D);

            auto *enemy = dynamic_cast<NPCEnemyBody *> (object);
            if (enemy != nullptr) {
                Vector3D vector = Vector3D(this->projectile->getPosition(), enemy->getPosition());
                int distance = vector.getComponent().getModule();
                if (distance <= projectile->getDamageRadius()) {
                    killEnemy(enemy);
                    cont++;
                }
            }
        }
        Logging::Log("makeDamageRadius: make damage for " + std::to_string(cont) + " enemies", "Collision");
    }

    void killEnemy(NPCEnemyBody *enemyBody) {
        bool explosionBody = enemyBody->isTakeHeavyDamage(projectile->getDamage());
        enemyBody->takeDamage(projectile->getDamage());

        if (enemyBody->stamina <= 0) {
            ComponentsManager::get()->getComponentGame()->kills++;
            enemyBody->setDead(true);
            enemyBody->state = EnemyState::ENEMY_STATE_DIE;

            // Set animation NPC to Dead
            SpriteDirectional3D *sprite = dynamic_cast<SpriteDirectional3D *> (enemyBody);
            sprite->setAnimation(EngineSetup::getInstance()->SpriteSoldierAnimations::SOLDIER_DEAD);

            if (explosionBody) {
                Tools::playMixedSound(EngineBuffers::getInstance()->soundPackage->getSoundByLabel("bodyExplode"),
                                      EngineSetup::SoundChannels::SND_EXPLODE_ENEMY, 0);

                for (int i = 0; i < 5; i++) {
                    gibsParticles(enemyBody);
                }
            }

            // remove object3D for check in stepSimulation
            dynamicsWorld->removeCollisionObject((btCollisionObject *) enemyBody->getRigidBody());

            // Offset down for draw sprite
            Vertex3D pos = enemyBody->getPosition();
            pos.y += 0.85f;
            enemyBody->setPosition(pos);
            enemyBody->setBodyEnabled(false);

            // Dead sound
            Tools::playMixedSound(EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundEnemyDead"),
                                  EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);

            // Remove sprite enemy from world
            //sprite->setRemoved(true);
        } else {
            enemyBody->state = EnemyState::ENEMY_STATE_INJURIED;
        }

        // Remove projectile for check in stepSimulation
        dynamicsWorld->removeCollisionObject(getProjectile()->getRigidBody());

        // Remove projectile from projectile list
        getProjectile()->setRemoved(true);

        // gore sprite "particles"
        Sprite3D *gore = new Sprite3D();
        gore->linkTextureAnimation(EngineBuffers::getInstance()->bloodTemplates);
        gore->setAutoRemoveAfterAnimation(true);
        gore->setPosition(getProjectile()->getPosition());
        gore->setAnimation(Tools::random(0, gore->numAnimations - 1));
        gore->getBillboard()->setDimensions(5, 5);
        Brakeza3D::get()->addObject3D(gore, "gore");
    }

    void gibsParticles(NPCEnemyBody *enemy) {
        Vertex3D position = enemy->getPosition();
        position.y -= 0.5;

        NPCEnemyPartBody *gibsBody = new NPCEnemyPartBody();
        gibsBody->setLabel("gibsBody");
        gibsBody->linkTextureAnimation(EngineBuffers::getInstance()->gibsTemplate);
        gibsBody->setAnimation(Tools::random(0, EngineBuffers::getInstance()->gibsTemplate->numAnimations - 1));
        gibsBody->setPosition(position);
        gibsBody->makeRigidBody(1.0f, dynamicsWorld);
        Brakeza3D::get()->addObject3D(gibsBody, gibsBody->getLabel());
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H
