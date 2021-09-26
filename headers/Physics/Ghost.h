//
// Created by darkhead on 8/2/21.
//

#ifndef BRAKEDA3D_GHOST_H
#define BRAKEDA3D_GHOST_H


#include "BillboardBody.h"
#include "../Objects/Mesh3D.h"

class Ghost {
public:
    Ghost();

    bool ghostEnabled = true;

    btPairCachingGhostObject *getGhostObject() { return ghostObject; };
    btPairCachingGhostObject *ghostObject;

    void makeGhostBody(Camera3D *cam, btDiscreteDynamicsWorld *world, bool useObjectSpace, Mesh3D *mesh);

    bool CheckGhost(btPairCachingGhostObject *Ghost);

    bool isGhostEnabled();

    void setGhostEnabled(bool enabled);

    virtual void integrate() = 0;
};


#endif //BRAKEDA3D_GHOST_H
