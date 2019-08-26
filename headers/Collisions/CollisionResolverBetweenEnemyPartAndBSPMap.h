#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENENEMYPARTANDBSPMAP_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENENEMYPARTANDBSPMAP_H


#include "../Render/BSPMap.h"
#include "CollisionResolver.h"
#include "../Render/Logging.h"
#include "../PhysicsGame/NPCEnemyPartBody.h"
#include "../../src/Decal.h"

class CollisionResolverBetweenEnemyPartAndBSPMap : public CollisionResolver {
public:
    BSPMap *bapMap;
    NPCEnemyPartBody *enemyPart;

    std::vector<Object3D *> *gameObjects;
    btDiscreteDynamicsWorld* dynamicsWorld;
    WeaponsManager *weaponManager;

    CollisionResolverBetweenEnemyPartAndBSPMap(Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Object3D *> *gameObjects, btDiscreteDynamicsWorld* dynamicsWorld, WeaponsManager *weaponManager) : CollisionResolver(objA, objB, bspMap)
    {
        this->enemyPart = getEnemyPart();
        this->bspMap = getBSPMap();

        this->gameObjects = gameObjects;
        this->dynamicsWorld = dynamicsWorld;
        this->weaponManager = weaponManager;
    }

    void dispatch()
    {
        if (EngineSetup::getInstance()->LOG_COLLISION_OBJECTS) {
            Logging::getInstance()->Log("CollisionResolverBetweenEnemyPartAndBSPMap");
        }

        if (enemyPart->doneGore) return;

        enemyPart->doneGore = true;

        // decal
        Decal *decal = new Decal(*enemyPart->getPosition());
        decal->texture->loadTGA( std::string(EngineSetup::getInstance()->IMAGES_FOLDER + "gore1.png").c_str(), 1 );
        decal->frustum->setup(
                *decal->getPosition(),
                Vertex3D(0, 0, 1),
                EngineSetup::getInstance()->up,
                EngineSetup::getInstance()->right,
                1,
                decal->h, decal->h,
                decal->w,
                decal->h, decal->h
        );
        decal->getTriangles(brakeza3D->visibleTriangles, brakeza3D->numVisibleTriangles, brakeza3D->camera),
        brakeza3D->addObject3D(decal, "NPCEnemyPartDecal");

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

    NPCEnemyPartBody *getEnemyPart()
    {
        NPCEnemyPartBody *NPCPartA = dynamic_cast<NPCEnemyPartBody*> (this->objA);
        if (NPCPartA != NULL) {
            return NPCPartA;
        }

        NPCEnemyPartBody *NPCPartB = dynamic_cast<NPCEnemyPartBody*> (this->objB);
        if (NPCPartB != NULL) {
            return NPCPartB;
        }
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENENEMYPARTANDBSPMAP_H
