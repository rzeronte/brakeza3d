//
// Created by darkhead on 2019-05-11.
//
#ifndef BRAKEDA3D_OBJECTPHYSIC_H
#define BRAKEDA3D_OBJECTPHYSIC_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../headers/Objects/Vertex3D.h"
#include "../headers/Objects/Mesh3D.h"


class Mesh3DPhysic: public Mesh3D {
public:
    Mesh3DPhysic();

    void integrate(Camera3D *);
    btRigidBody* getRigidBody() { return m_body;}
    btCollisionObject* getCollider() { return m_collider;}
    void disableRotation() { m_disableRotation = true ;}

    btRigidBody* makeRigidBody(std::vector<Mesh3DPhysic *> &, Camera3D *cam, btDiscreteDynamicsWorld*);
    float mass = 10.f;

private:

    bool m_isActive;
    bool m_disableRotation;
    btMotionState* motion;
    btRigidBody* m_body;
    btCollisionObject* m_collider;
};


#endif //BRAKEDA3D_OBJECTPHYSIC_H
