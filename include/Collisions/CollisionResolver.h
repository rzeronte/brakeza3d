
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

    std::vector<Triangle *> *visibleTriangles;

    int type;

    CollisionResolver(btPersistentManifold *contactManifold, Object3D *objA, Object3D *objB,
                      std::vector<Triangle *> &visibleTriangles);

    int getTypeCollision() const;

    bool isSomeCamera() const;

    bool isSomeNPCEnemy() const;

    bool isSomeNPCEnemyPart() const;

    bool isSomeProjectile() const;

    bool isSomeMesh3D() const;

    bool isSomeMesh3DFuncDoor() const;

    bool isSomeMesh3DFuncButton(BSPMap *bspMap) const;

    bool isSomeMesh3DTriggerMultiple(BSPMap *bspMap) const;

    bool isSomeMesh3DTriggerTeleport(BSPMap *bspMap) const;

    bool isSomeItemWeapon() const;

    bool isSomeItemHealth() const;

    bool isSomeItemAmmo() const;

    bool isBSPEntityOfClassName(BSPMap *bspMap, Mesh3D *oMesh, const std::string& query) const;

    void moveDoorGhost(BSPMap *bspMap, DoorGhost *oRemoteBody, int targetEntityId) const;

    int getType() const;

    void setType(int type);
};


#endif //BRAKEDA3D_COLLISIONRESOLVER_H
