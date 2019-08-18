
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDNPCENEMY_H


#include "CollisionResolver.h"
#include "../Physics/Projectile3DBody.h"
#include "../Physics/NPCEnemyBody.h"

class CollisionResolverBetweenProjectileAndNPCEnemy : public CollisionResolver {

public:
    Projectile3DBody *projectile;
    NPCEnemyBody *npcEnemy;

    CollisionResolverBetweenProjectileAndNPCEnemy(Object3D *objA, Object3D *objB, BSPMap *bspMap) : CollisionResolver(objA, objB, bspMap)
    {
        this->projectile = this->getProjectile();
        this->npcEnemy   = this->getNPCEnemy();
    }

    void dispatch()
    {
        Logging::getInstance()->Log("CollisionResolverBetweenProjectileAndNPCEnemy");
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
