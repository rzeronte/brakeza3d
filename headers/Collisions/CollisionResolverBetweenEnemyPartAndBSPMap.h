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

        getEnemyPart()->getCurrentTextureAnimation()->setPaused(true);

        makeGoreDecals(90, 0, 0);
        makeGoreDecals(-90, 0, 0);
        makeGoreDecals(0, 0, 0);
        makeGoreDecals(0, 90, 0);
        makeGoreDecals(0, 180, 0);
        makeGoreDecals(0, -90, 0);
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

    void makeGoreDecals(float rotX, float rotY, float rotZ) {
        Decal *decal = new Decal();
        decal->setPosition(*getEnemyPart()->getPosition());
        decal->setupCube(5, 5, 5);
        decal->setRotation(M3::getMatrixRotationForEulerAngles(rotX, rotY, rotZ));
        decal->getSprite()->linkTextureAnimation(EngineBuffers::getInstance()->goreTemplate);
        decal->cube->setPosition(*decal->getPosition());
        decal->cube->update();
        decal->getTriangles(brakeza3D->visibleTriangles, brakeza3D->numVisibleTriangles, brakeza3D->camera);
        brakeza3D->addObject3D(decal, "decal");
    }
};


#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENENEMYPARTANDBSPMAP_H
