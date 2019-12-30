
#ifndef BRAKEDA3D_NPCENEMYPARTBODY_H
#define BRAKEDA3D_NPCENEMYPARTBODY_H


#include "Enemy.h"
#include "../Physics/Sprite3DBody.h"

class NPCEnemyPartBody : public Sprite3DBody {
public:
    bool doneGore = false;

    btRigidBody* makeRigidBody(float, std::vector<Object3D*> &, btDiscreteDynamicsWorld*);

};


#endif //BRAKEDA3D_NPCENEMYPARTBODY_H
