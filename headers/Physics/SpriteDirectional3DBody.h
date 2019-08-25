
#ifndef BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H
#define BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H


#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../Render/Timer.h"
#include "../Objects/SpriteDirectional3D.h"
#include "Body.h"

class SpriteDirectional3DBody : public SpriteDirectional3D, public Body {
public:
    btRigidBody* makeRigidBody(float, std::vector<Object3D*> &, Camera3D *, btDiscreteDynamicsWorld*, bool applyCameraImpulse, int forceImpulse);
    void integrate();

};


#endif //BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H
