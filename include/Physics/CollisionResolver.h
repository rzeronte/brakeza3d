
#ifndef BRAKEDA3D_COLLISIONRESOLVER_H
#define BRAKEDA3D_COLLISIONRESOLVER_H

#include "../Objects/Object3D.h"
#include "../Objects/Mesh3D.h"
#include "../Render/BSPMap.h"
#include "Mesh3DBody.h"
#include "Projectile3DBody.h"
#include "../Game/DoorGhost.h"

class CollisionResolver {

public:
    Body *objA;
    Body *objB;

    std::vector<Triangle *> *visibleTriangles;

    CollisionResolver(
        Body *objA,
        Body *objB,
        std::vector<Triangle *> &visibleTriangles
    );

    bool isBSPEntityOfClassName(BSPMap *bspMap, Mesh3D *oMesh, const std::string& query) const;
};


#endif //BRAKEDA3D_COLLISIONRESOLVER_H
