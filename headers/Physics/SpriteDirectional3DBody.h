
#ifndef BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H
#define BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H


#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../Misc/Timer.h"
#include "../Objects/SpriteDirectional3D.h"
#include "Body.h"

class SpriteDirectional3DBody : public SpriteDirectional3D, public Body {
public:
    btRigidBody* makeRigidBody(float mass, Vertex3D size, std::vector<Object3D*> &, btDiscreteDynamicsWorld*);
    btRigidBody* makeProjectileRigidBody(float mass, Vertex3D size, std::vector<Object3D*> &, Camera3D *, btDiscreteDynamicsWorld*, bool applyCameraImpulse, float forceImpulse, float accuracy);
    btRigidBody* makeProjectileRigidBodyToPlayer(float mass, Vertex3D size, std::vector<Object3D*> &gameObjects, Vertex3D dir, btDiscreteDynamicsWorld*, float forceImpulse);

    void integrate();

};


#endif //BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H
