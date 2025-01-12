
#ifndef BRAKEDA3D_SPRITE3DBODY_H
#define BRAKEDA3D_SPRITE3DBODY_H

#include <btBulletDynamicsCommon.h>
#include "../Objects/BillboardAnimation.h"
#include "Body.h"

class BillboardAnimationBody : public BillboardAnimation, public Body {
public:
    btRigidBody *makeRigidBody(float mass, Vertex3D size, btDiscreteDynamicsWorld *);

    void integrate();

};


#endif //BRAKEDA3D_SPRITE3DBODY_H
