
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Physics/Mesh3DAnimatedGhost.h"
#include "../weapons/AmmoProjectileBodyEmitter.h"
#include "../../../include/Physics/RayCollisionable.h"
#include "../../../include/Physics/ProjectileRay.h"
#include "../shaders/ShaderZombie.h"
#include "../items/EnemyDialog.h"
#include "../behaviors/RotatableToTarget.h"

class EnemyGhost : public Mesh3DAnimatedGhost, public Enemy, public LivingObject, public RotatableToTarget {
private:
    ShaderZombie *zombie;

    RayLight rayLight;
    Counter counterStuck;
    AmmoProjectileBodyEmitter *projectileEmitter;
    std::vector<ProjectileRay *> fixedLasers;

    ParticleEmitter *particleEmitter;
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

    void addFixedLaser(ProjectileRay *ray);

    void updateLasers();

    void setProjectileEmitter(AmmoProjectileBodyEmitter *emitter);

    Object3D *getTarget();

    [[nodiscard]] AmmoProjectileBodyEmitter *getProjectileEmitter() const;

    void handleDie();

    void onDrawHostBuffer() override;

    void drawOnUpdateSecondPass() override;

    std::vector<EnemyDialog *> dialogs;

    void takeDamage(float damageTaken);

    void updateEmitterParticles();
};


#endif //BRAKEDA3D_ENEMYGHOST_H
