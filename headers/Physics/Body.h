
#ifndef BRAKEDA3D_BODY_H
#define BRAKEDA3D_BODY_H

#include <btBulletDynamicsCommon.h>
#include "../Objects/Vertex3D.h"

class Body {

public:

    Body();

    float mass = 0;
    bool  bodyEnabled = true;

    Vertex3D boxShapeSize = Vertex3D(1, 1, 1);

    btRigidBody* m_body;
    btCollisionObject* m_collider;

    btCollisionObject* getCollider()  { return m_collider; }
    btRigidBody*       getRigidBody() { return m_body; }

    void setBodyEnabled(bool state);
    void setBoxShapeSize(Vertex3D size);
    Vertex3D getBoxShapeSize();

    virtual void integrate();


};


#endif //BRAKEDA3D_BODY_H
