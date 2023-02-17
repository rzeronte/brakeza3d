
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Shaders/ShaderBlink.h"
#include "../shaders/ShaderTrailObject.h"
#include "../../../include/Physics/Mesh3DAnimatedGhost.h"
#include "../weapons/AmmoProjectileBodyEmissor.h"
#include "../../../include/Physics/RayCollisionable.h"
#include "../../../include/Physics/ProjectileRay.h"

class EnemyGhost : public Mesh3DAnimatedGhost, public Enemy {
private:
    ShaderBlink *blink;
    ShaderLaser *laser;
    Counter *counterDamageBlink;
    Counter *counterStucked;
    AmmoProjectileBodyEmissor *projectileEmissor;

    std::vector<ProjectileRay *> fixedLasers;
public:
    EnemyGhost();

    void resolveCollision(Collisionable *collisionableObject) override;

    void onUpdate() override;

    void integrate() override;

    void remove() override;

    void shoot(Object3D *target);

    [[nodiscard]] ShaderBlink *getBlink() const;

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

    [[nodiscard]] AmmoProjectileBodyEmissor *getProjectileEmissor() const;

    void addFixedLaser(ProjectileRay *ray);

    void updateLasers();
};


#endif //BRAKEDA3D_ENEMYGHOST_H
