
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Shaders/ShaderBlink.h"
#include "../shaders/ShaderTrailObject.h"
#include "../../../include/Physics/Mesh3DAnimatedGhost.h"
#include "../weapons/AmmoProjectileBodyEmissor.h"

class EnemyGhost : public Mesh3DAnimatedGhost, public Enemy {
private:
    ShaderBlink *blink;
    ShaderLaser *laser;
    Counter *counterDamageBlink;
    Counter *counterStucked;

    AmmoProjectileBodyEmissor *projectileEmissor;

public:
    EnemyGhost();

    void resolveCollision(Collisionable *collisionableObject) override;

    virtual void onUpdate() override;

    void integrate() override;

    void remove();

    void shoot(Object3D *target);

    ShaderBlink *getBlink() const;

    void postUpdate() override;

    void rotateToTarget();

    void makeReward();

    ~EnemyGhost() override;

    void loadBlinkShader();

    void stuck(float time);

    void unstuck();

    void makeExplosion();

    Object3D *getTarget();

    void setProjectileEmissor(AmmoProjectileBodyEmissor *projectileEmissor);

    AmmoProjectileBodyEmissor *getProjectileEmissor() const;
};


#endif //BRAKEDA3D_ENEMYGHOST_H
