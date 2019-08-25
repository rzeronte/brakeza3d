#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H


#include "../GamePhysics/Projectile3DBody.h"
#include "../Render/BSPMap.h"
#include "CollisionResolver.h"
#include "../Render/Logging.h"

class CollisionResolverBetweenEnemyPartAndBSPMap : public CollisionResolver {
public:
    BSPMap *bapMap;
    Sprite3D *enemyPart;

    std::vector<SpriteDirectional3DBody *> *projectiles;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenEnemyPartAndBSPMap(Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<SpriteDirectional3DBody *> *projectiles, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager) : CollisionResolver(objA, objB, bspMap)
    {
        this->enemyPart = getEnemyPart();
        this->bspMap = getBSPMap();

        this->projectiles = projectiles;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenEnemyPartAndBSPMap");
        }
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

    Sprite3D *getEnemyPart()
    {
        Sprite3D *projectileA = dynamic_cast<Sprite3D*> (this->objA);
        if (projectileA != NULL) {
            return projectileA;
        }

        Sprite3D *projectileB = dynamic_cast<Sprite3D*> (this->objB);
        if (projectileB != NULL) {
            return projectileB;
        }
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENPROJECTILEANDBSPMAP_H
