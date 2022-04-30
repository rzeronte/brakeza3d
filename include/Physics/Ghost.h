#ifndef BRAKEDA3D_GHOST_H
#define BRAKEDA3D_GHOST_H


#include "BillboardBody.h"
#include "../Objects/Mesh3D.h"

class Ghost: public Collisionable {
public:
    Ghost();

    [[nodiscard]] btPairCachingGhostObject *getGhostObject() const;
    btPairCachingGhostObject *ghostObject;
    btConvexHullShape *convexHullShape;

    void makeGhostBody(btDiscreteDynamicsWorld *world, Mesh3D *mesh, int collisionGroup, int collisionMask);
    void removeCollisionObject() const;

    bool CheckGhost(btPairCachingGhostObject *Ghost);

    virtual ~Ghost();

    void makeSimpleGhostBody(
        Vertex3D pos,
        Vertex3D dimensions,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
    );
};


#endif //BRAKEDA3D_GHOST_H
