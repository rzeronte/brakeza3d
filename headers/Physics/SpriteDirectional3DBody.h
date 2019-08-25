
#ifndef BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H
#define BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H


#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../Render/Timer.h"
#include "../Objects/SpriteDirectional3D.h"
#include "../Objects/Mesh3D.h"
#include "Mesh3DBody.h"

class SpriteDirectional3DBody : public SpriteDirectional3D, public Body {
public:
    SpriteDirectional3DBody();

    btRigidBody* makeRigidBody(float, std::vector<Object3D*> &, Camera3D *, btDiscreteDynamicsWorld*, bool applyCameraImpulse, int forceImpulse);
    void integrate();

};


#endif //BRAKEDA3D_SPRITEDIRECTIONAL3DBODY_H
