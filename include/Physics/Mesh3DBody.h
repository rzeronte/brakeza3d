#ifndef BRAKEDA3D_OBJECTPHYSIC_H
#define BRAKEDA3D_OBJECTPHYSIC_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../Objects/Vertex3D.h"
#include "../Objects/Mesh3D.h"
#include "Body.h"


class Mesh3DBody : public Mesh3D, public Body {
public:
    Mesh3DBody();

    void integrate() override;

    void makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *, int collisionGroup, int collisionMask);

    void makeRigidBody(float mass, btDiscreteDynamicsWorld *, int collisionGroup, int collisionMask);

    void makeSimpleRigidBody(float mass, Vertex3D pos, Vertex3D dimensions, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask);

    void setGravity(Vertex3D g);

    void remove();

private:
    btConvexHullShape *getConvexHullShapeFromMesh();

    btBvhTriangleMeshShape *getTriangleMeshFromMesh();

protected:
    void resolveCollision(Collisionable *collisionable) override;

};


#endif //BRAKEDA3D_OBJECTPHYSIC_H
