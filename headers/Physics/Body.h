
#ifndef BRAKEDA3D_BODY_H
#define BRAKEDA3D_BODY_H

#include <btBulletDynamicsCommon.h>

class Body {

public:

    Body();

    float mass;

    bool bodyEnabled = true;

    btRigidBody* m_body;
    btCollisionObject* m_collider;

    btCollisionObject* getCollider() { return m_collider; }
    btRigidBody* getRigidBody() { return m_body; }
    virtual void integrate();
};


#endif //BRAKEDA3D_BODY_H
