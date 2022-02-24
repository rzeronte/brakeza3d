#ifndef BRAKEDA3D_GHOST_H
#define BRAKEDA3D_GHOST_H


#include "BillboardBody.h"
#include "../Objects/Mesh3D.h"

class Ghost: public Collisionable {
public:
    Ghost();

    btPairCachingGhostObject *getGhostObject() { return ghostObject; };
    btPairCachingGhostObject *ghostObject;

    void makeGhostBody(btDiscreteDynamicsWorld *world, Mesh3D *mesh);

    bool CheckGhost(btPairCachingGhostObject *Ghost);
};


#endif //BRAKEDA3D_GHOST_H
