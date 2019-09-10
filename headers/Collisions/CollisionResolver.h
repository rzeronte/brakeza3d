
#ifndef BRAKEDA3D_COLLISIONRESOLVER_H
#define BRAKEDA3D_COLLISIONRESOLVER_H

#include "../Objects/Object3D.h"
#include "../Objects/Mesh3D.h"
#include "../Render/BSPMap.h"
#include "../Physics/Mesh3DBody.h"
#include "../PhysicsGame/Projectile3DBody.h"
#include "../Render/Engine.h"

extern Engine *brakeza3D;

class CollisionResolver {

public:
    btPersistentManifold *contactManifold;

    Object3D *objA;
    Object3D *objB;
    BSPMap *bspMap;

    CollisionResolver(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap);
    virtual ~CollisionResolver();

    int getTypeCollision();

    bool isSomeCamera();
    bool isSomeBSPMap();
    bool isSomeNPCEnemy();
    bool isSomeNPCEnemyPart();
    bool isSomeProjectile();
    bool isSomeMesh3D();
    bool isSomeMesh3dFuncDoor();
    bool isSomeMesh3dFuncButton();

    bool isBSPEntityOfClassName(Mesh3D *oMesh, std::string query);
    void moveMesh3DBody(Mesh3DBody *oRemoteBody, int targetEntityId);
};


#endif //BRAKEDA3D_COLLISIONRESOLVER_H
