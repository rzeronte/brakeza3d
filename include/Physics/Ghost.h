#ifndef BRAKEDA3D_GHOST_H
#define BRAKEDA3D_GHOST_H


#include "BillboardBody.h"
#include "../Objects/Mesh3D.h"

class Ghost: public Collisionable {
public:
    Ghost();

    btPairCachingGhostObject *getGhostObject() const;
    btPairCachingGhostObject *ghostObject;

    void makeGhostBody(btDiscreteDynamicsWorld *world, Mesh3D *mesh, int collisionGroup, int collisionMask);
    void removeCollisionObject() const;

    bool CheckGhost(btPairCachingGhostObject *Ghost);

    virtual ~Ghost();
};


#endif //BRAKEDA3D_GHOST_H
