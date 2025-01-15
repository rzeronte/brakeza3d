#ifndef BRAKEDA3D_GHOST_H
#define BRAKEDA3D_GHOST_H


#include "BillboardBody.h"
#include "../Objects/Mesh3D.h"

enum GhostTypeShape {
    GHOST_SIMPLE_SHAPE = 0,
    GHOST_TRIANGLE3D_MESH_SHAPE = 1
};

class Ghost: public Collisionable {
protected:
    GhostTypeShape typeShape;
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
        Vertex3D dimensions,
        btDiscreteDynamicsWorld *world,
        int collisionGroup,
        int collisionMask
    );

    [[nodiscard]] GhostTypeShape getTypeShape() const;

    void setTypeShape(GhostTypeShape typeShape);

    Vertex3D simpleShapeSize;
};


#endif //BRAKEDA3D_GHOST_H
