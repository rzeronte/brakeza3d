//
// Created by darkhead on 2019-05-31.
//

#ifndef BRAKEDA3D_MESH3DGHOST_H
#define BRAKEDA3D_MESH3DGHOST_H


#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../Objects/Vertex3D.h"
#include "../Objects/Mesh3D.h"

class Mesh3DGhost: public Mesh3D {
public:
    Mesh3DGhost();

    void makeGhostBody(Camera3D *cam, btDiscreteDynamicsWorld*, bool useObjectSpace);
    static bool CheckGhost(btPairCachingGhostObject* Ghost);

    btPairCachingGhostObject* getGhostObject() { return m_ghostObject; };

    btPairCachingGhostObject *m_ghostObject;

    int currentTriggerCounter = 0;

private:

};
#endif //BRAKEDA3D_MESH3DGHOST_H
