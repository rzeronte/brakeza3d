
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Shaders/ShaderBlink.h"
#include "../shaders/ShaderTrailObject.h"
#include "../../../include/Physics/Mesh3DAnimatedGhost.h"

class EnemyGhost : public Mesh3DAnimatedGhost, public Enemy {
private:
    ShaderBlink *blink;
    Counter *counterDamageBlink;
    Counter *counterStucked;
public:
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

    ~EnemyGhost() override;

    void loadBlinkShader();

    void stuck(float time);

    void unstuck();

    void makeExplosion();
};


#endif //BRAKEDA3D_ENEMYGHOST_H
