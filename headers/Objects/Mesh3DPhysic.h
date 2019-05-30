//
// Created by darkhead on 2019-05-11.
//
#ifndef BRAKEDA3D_OBJECTPHYSIC_H
#define BRAKEDA3D_OBJECTPHYSIC_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "Vertex3D.h"
#include "Mesh3D.h"


class Mesh3DPhysic: public Mesh3D {
public:
    Mesh3DPhysic();

    void integrate();
    btRigidBody* getRigidBody() { return m_body; }
    btCollisionObject* getCollider() { return m_collider; }
    void disableRotation() { m_disableRotation = true ; }

    btRigidBody* makeRigidBody(float mass, std::vector<Mesh3DPhysic *> &, Camera3D *cam, btDiscreteDynamicsWorld*,  bool useObjectSpace);
    btRigidBody* makeGhostBody(Camera3D *cam, btDiscreteDynamicsWorld*,  bool useObjectSpace);
    static bool CheckGhost(btPairCachingGhostObject* Ghost);

    btPairCachingGhostObject* getGhostObject() { return m_ghostObject; };
    btRigidBody* getRigidObject() { return m_body; };

    float mass = 10.f;

    btRigidBody* m_body;
    btPairCachingGhostObject *m_ghostObject;

private:

    bool m_isActive;
    bool m_disableRotation;
    btMotionState* motion;

    btCollisionObject* m_collider;
};


#endif //BRAKEDA3D_OBJECTPHYSIC_H
