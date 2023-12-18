
#ifndef BRAKEDA3D_BODY_H
#define BRAKEDA3D_BODY_H

#include <btBulletDynamicsCommon.h>
#include "../Objects/Vertex3D.h"
#include "../../src/Collisions/Collisionable.h"

enum BodyTypeShape {
    BODY_SIMPLE_SHAPE = 0,
    BODY_TRIANGLE3D_MESH_SHAPE = 1
};

class Body: public Collisionable {
protected:
    BodyTypeShape typeShape;
public:
    Body();

    float mass;

    btRigidBody *body;

    [[nodiscard]] btRigidBody *getRigidBody() const;

    void setMass(float m);

    void removeCollisionObject() const;

    [[nodiscard]] BodyTypeShape getTypeShape() const;

    void setTypeShape(BodyTypeShape typeShape);

    virtual ~Body();
};


#endif //BRAKEDA3D_BODY_H
