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

    void makeRigidBodyFromTriangleMesh(float mass, btDiscreteDynamicsWorld *);

    void makeRigidBody(float mass, btDiscreteDynamicsWorld *);

    void makeSimpleRigidBody(float mass, Vertex3D pos, Vertex3D dimensions, btDiscreteDynamicsWorld *world);

    void setGravity(Vertex3D g);
private:
    btConvexHullShape *getConvexHullShapeFromMesh();

    btBvhTriangleMeshShape *getTriangleMeshFromMesh();

protected:
    void resolveCollision(Collisionable *collisionable) override;

    void remove();
};


#endif //BRAKEDA3D_OBJECTPHYSIC_H
