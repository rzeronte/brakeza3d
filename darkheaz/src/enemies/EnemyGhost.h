
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Shaders/ShaderBlink.h"
#include "../../../include/Physics/Mesh3DAnimatedGhost.h"
#include "../weapons/AmmoProjectileBodyEmitter.h"
#include "../../../include/Physics/RayCollisionable.h"
#include "../../../include/Physics/ProjectileRay.h"

class EnemyGhost : public Mesh3DAnimatedGhost, public Enemy {
private:
    ShaderBlink *blink;
    RayLight rayLight;
    Counter counterDamageBlink;
    Counter counterStuck;
    AmmoProjectileBodyEmitter *projectileEmitter;
    std::vector<ProjectileRay *> fixedLasers;
public:
    EnemyGhost();

    ~EnemyGhost() override;

    void resolveCollision(Collisionable *withObject) override;

    void onStart();

    void onUpdate() override;

    void integrate() override;

    void remove() override;

    void shoot(Object3D *target);

    void postUpdate() override;

    void rotateToTarget();

    void makeReward();

    void stuck(float time);

    void unstuck();

    void makeExplosion();

    void addFixedLaser(ProjectileRay *ray);

    void updateLasers();

    void setProjectileEmitter(AmmoProjectileBodyEmitter *emitter);

    Object3D *getTarget();

    [[nodiscard]] AmmoProjectileBodyEmitter *getProjectileEmitter() const;

    [[nodiscard]] ShaderBlink *getBlink();

    void handleDie();
};


#endif //BRAKEDA3D_ENEMYGHOST_H
