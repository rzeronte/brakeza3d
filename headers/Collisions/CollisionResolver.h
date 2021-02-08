
#ifndef BRAKEDA3D_COLLISIONRESOLVER_H
#define BRAKEDA3D_COLLISIONRESOLVER_H

#include "../Objects/Object3D.h"
#include "../Objects/Mesh3D.h"
#include "../Render/BSPMap.h"
#include "../Physics/Mesh3DBody.h"
#include "../Game/Projectile3DBody.h"
#include "../Game/DoorGhost.h"

class CollisionResolver {

public:
    btPersistentManifold *contactManifold;

    Object3D *objA;
    Object3D *objB;
    BSPMap *bspMap;

    std::vector<Triangle *> *visibleTriangles;

    int type;

    CollisionResolver(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB, BSPMap *bspMap, std::vector<Triangle *> &visibleTriangles);
    virtual ~CollisionResolver();

    int getTypeCollision();

    bool isSomeCamera();
    bool isSomeNPCEnemy();
    bool isSomeNPCEnemyPart();
    bool isSomeProjectile();
    bool isSomeMesh3D();
    bool isSomeMesh3DFuncDoor();
    bool isSomeMesh3DFuncButton();
    bool isSomeMesh3DTriggerMultiple();
    bool isSomeMesh3DTriggerTeleport();
    bool isSomeItemWeapon();
    bool isSomeItemHealth();
    bool isSomeItemAmmo();

    bool isBSPEntityOfClassName(Mesh3D *oMesh, std::string query);
    void moveDoorGhost(DoorGhost *oRemoteBody, int targetEntityId);

    int  getType() const;
    void setType(int type);
};


#endif //BRAKEDA3D_COLLISIONRESOLVER_H
