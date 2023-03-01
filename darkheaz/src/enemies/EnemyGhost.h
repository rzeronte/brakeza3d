
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Shaders/ShaderBlink.h"
#include "../../../include/Physics/Mesh3DAnimatedGhost.h"
#include "../weapons/AmmoProjectileBodyEmissor.h"
#include "../../../include/Physics/RayCollisionable.h"
#include "../../../include/Physics/ProjectileRay.h"

class EnemyGhost : public Mesh3DAnimatedGhost, public Enemy {
private:
    ShaderBlink *blink;
    ShaderLightRay *laser;
    Counter counterDamageBlink;
    Counter counterStucked;
    AmmoProjectileBodyEmissor *projectileEmissor;
    std::vector<ProjectileRay *> fixedLasers;
public:
    EnemyGhost();

    ~EnemyGhost() override;

    void resolveCollision(Collisionable *withObject) override;

    void onUpdate() override;

    void integrate() override;

    void remove() override;

    void shoot(Object3D *target);

    void postUpdate() override;

    void rotateToTarget();

    void makeReward();

    void loadBlinkShader();

    void stuck(float time);

    void unstuck();

    void makeExplosion();

    void addFixedLaser(ProjectileRay *ray);

    void updateLasers();

    void setProjectileEmissor(AmmoProjectileBodyEmissor *projectileEmissor);

    Object3D *getTarget();

    [[nodiscard]] AmmoProjectileBodyEmissor *getProjectileEmissor() const;

    [[nodiscard]] ShaderBlink *getBlink() const;

};


#endif //BRAKEDA3D_ENEMYGHOST_H
