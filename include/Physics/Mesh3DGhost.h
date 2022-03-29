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
    bool free;
    int currentTriggerCounter = 0;
private:
    void updateBulletFromMesh3D();

protected:
    void integrate() override;

public:
    void resolveCollision(Collisionable *collisionable) override;

    void remove();

    bool isFree() const;

    void setFree(bool free);
};

#endif //BRAKEDA3D_MESH3DGHOST_H
