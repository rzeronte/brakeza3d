
#ifndef BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H
#define BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H


#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../Render/Timer.h"
#include "../Objects/SpriteDirectional3D.h"
#include "../Objects/Mesh3D.h"
#include "Mesh3DBody.h"

class SpriteDirectional3DBody : public SpriteDirectional3D {
public:

    float mass = 0;

    btRigidBody* m_body;

    SpriteDirectional3DBody();

    btRigidBody* makeRigidBody(float, std::vector<SpriteDirectional3DBody*> &, Camera3D *, btDiscreteDynamicsWorld*, bool applyCameraImpulse, int forceImpulse);
    btRigidBody* getRigidBody() { return m_body; }
    btCollisionObject* getCollider() { return m_collider; }
    btRigidBody* getRigidObject() { return m_body; };

    void integrate();

private:
    btCollisionObject* m_collider;

};


#endif //BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H
