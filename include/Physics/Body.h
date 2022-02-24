
#ifndef BRAKEDA3D_BODY_H
#define BRAKEDA3D_BODY_H

#include <btBulletDynamicsCommon.h>
#include "../Objects/Vertex3D.h"
#include "../../src/Collisions/Collisionable.h"

class Body: public Collisionable {

public:
    Body();

    float mass;

    Vertex3D boxShapeSize;

    btRigidBody *body;
    btCollisionObject *collisionObject;

    btTriangleMesh triangleMesh;
    btBvhTriangleMeshShape *shape;
    btDefaultMotionState *motionState;

    btCollisionObject *getCollisionObject() const;

    btRigidBody *getRigidBody() const;

    void setBoxShapeSize(Vertex3D size);

    Vertex3D getBoxShapeSize() const;

    void applyImpulse(Vertex3D impulse) const;

    void setMass(float m);

};


#endif //BRAKEDA3D_BODY_H
