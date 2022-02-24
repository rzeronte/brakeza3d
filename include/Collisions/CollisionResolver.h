
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

    Body *objA;
    Body *objB;

    std::vector<Triangle *> *visibleTriangles;

    int type;

    CollisionResolver(
        btPersistentManifold *contactManifold,
        Body *objA,
        Body *objB,
        std::vector<Triangle *> &visibleTriangles
    );

    int getTypeCollision() const;

    bool isBSPEntityOfClassName(BSPMap *bspMap, Mesh3D *oMesh, const std::string& query) const;

    int getType() const;

    void setType(int type);

    virtual void dispatch() {};
};


#endif //BRAKEDA3D_COLLISIONRESOLVER_H
