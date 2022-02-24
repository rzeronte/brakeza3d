#ifndef BRAKEDA3D_MESH3DGHOST_H
#define BRAKEDA3D_MESH3DGHOST_H


#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../Objects/Vertex3D.h"
#include "../Objects/Mesh3D.h"
#include "Ghost.h"

class Mesh3DGhost : public Mesh3D, public Ghost {
public:
    Mesh3DGhost();

    int currentTriggerCounter = 0;
private:
    void integrate() override;
    void updateBulletFromMesh();

public:
    void dispatchCollision(Collisionable *collisionable) override;
};

#endif //BRAKEDA3D_MESH3DGHOST_H
