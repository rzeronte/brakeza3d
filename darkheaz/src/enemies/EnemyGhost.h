
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Shaders/ShaderBlink.h"

class EnemyGhost : public Mesh3DGhost, public Enemy {
public:

    ShaderBlink *blink;
    Counter *counterDamageBlink;
    EnemyGhost();

    void resolveCollision(Collisionable *collisionableObject) override;

    virtual void onUpdate() override;

    void integrate() override;

    void remove();

    void shoot(Object3D *target);

    ShaderBlink *getBlink() const;

    void postUpdate() override;

    void rotateToPlayer();

    void makeReward();
};


#endif //BRAKEDA3D_ENEMYGHOST_H