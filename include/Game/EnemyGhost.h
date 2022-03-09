
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../Physics/Mesh3DGhost.h"

class EnemyGhost : public Mesh3DGhost, public Enemy {

public:

    EnemyGhost();

    void shoot();

    void resolveCollision(Collisionable *collisionableObject) override;

    void onUpdate() override;

    void integrate() override;
};


#endif //BRAKEDA3D_ENEMYGHOST_H
