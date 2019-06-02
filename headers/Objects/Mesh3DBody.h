//
// Created by darkhead on 2019-05-11.
//
#ifndef BRAKEDA3D_OBJECTPHYSIC_H
#define BRAKEDA3D_OBJECTPHYSIC_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "Vertex3D.h"
#include "Mesh3D.h"


class Mesh3DBody: public Mesh3D {
public:
    Mesh3DBody();

    void integrate();
    btRigidBody* getRigidBody() { return m_body; }
    btCollisionObject* getCollider() { return m_collider; }
    void disableRotation() { m_disableRotation = true ; }

    btRigidBody* makeRigidBody(float mass, std::vector<Mesh3DBody *> &, Camera3D *cam, btDiscreteDynamicsWorld*, bool useObjectSpace);
    btRigidBody* getRigidObject() { return m_body; };

    float mass;
    btRigidBody* m_body;

private:

    bool m_isActive;
    bool m_disableRotation;
    btMotionState* motion;

    btCollisionObject* m_collider;
};


#endif //BRAKEDA3D_OBJECTPHYSIC_H
