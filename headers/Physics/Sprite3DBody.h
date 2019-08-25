
#ifndef BRAKEDA3D_SPRITE3DBODY_H
#define BRAKEDA3D_SPRITE3DBODY_H

#include <btBulletDynamicsCommon.h>
#include "../Objects/Sprite3D.h"
#include "Body.h"

class Sprite3DBody : public Sprite3D, public Body {
public:
    btRigidBody* makeRigidBody(float, std::vector<Object3D*> &, Camera3D *, btDiscreteDynamicsWorld*, bool applyCameraImpulse, int forceImpulse);
    void integrate();

};


#endif //BRAKEDA3D_SPRITE3DBODY_H
