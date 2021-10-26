
#ifndef BRAKEDA3D_SPRITE3DBODY_H
#define BRAKEDA3D_SPRITE3DBODY_H

#include <btBulletDynamicsCommon.h>
#include "../Objects/Sprite3D.h"
#include "Body.h"

class Sprite3DBody : public Sprite3D, public Body {
public:
    btRigidBody *makeRigidBody(float mass, Vertex3D size, btDiscreteDynamicsWorld *);

    void integrate();

};


#endif //BRAKEDA3D_SPRITE3DBODY_H
