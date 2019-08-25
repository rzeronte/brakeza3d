//
// Created by darkhead on 2019-08-25.
//

#ifndef BRAKEDA3D_BODY_H
#define BRAKEDA3D_BODY_H

#include <btBulletDynamicsCommon.h>
#include "../Objects/Camera3D.h"

class Body {

public:

    Body();

    //virtual void integrate();

    btRigidBody* makeRigidBodyForMesh3D(float mass, std::vector<Body *> &, Camera3D *cam, btDiscreteDynamicsWorld*, bool useObjectSpace);
    btRigidBody* makeRigidBodyForSpriteDirectional3D(float, std::vector<Body*> &, Camera3D *, btDiscreteDynamicsWorld*, bool applyCameraImpulse, int forceImpulse);

    float mass;

    btRigidBody* m_body;
    btCollisionObject* m_collider;

    btRigidBody* makeRigidBody(float, std::vector<Body*> &, Camera3D *, btDiscreteDynamicsWorld*, bool applyCameraImpulse, int forceImpulse);

    btCollisionObject* getCollider() { return m_collider; }
    btRigidBody* getRigidObject() { return m_body; };

    btRigidBody* getRigidBody() { return m_body; }
};


#endif //BRAKEDA3D_BODY_H
