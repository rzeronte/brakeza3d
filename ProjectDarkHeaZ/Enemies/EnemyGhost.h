
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/Physics/Mesh3DAnimatedGhost.h"
#include "../Weapons/AmmoProjectileBodyEmitter.h"
#include "../../include/Physics/RayCollisionable.h"
#include "../Shaders/ShaderZombie.h"
#include "../Items/EnemyDialog.h"
#include "../ProjectileRay.h"
#include "../../include/Behaviors/RotatableToTarget.h"
#include "../../include/Objects/Swarm.h"
#include "../../include/Objects/TentacleIK.h"

class EnemyGhost : public Mesh3DAnimatedGhost, public Enemy, public LivingObject, public RotatableToTarget {
private:
    ShaderZombie *zombie;

    RayLight rayLight;
    Counter counterStuck;
    AmmoProjectileBodyEmitter *projectileEmitter;

    ParticleEmitter *particleEmitter;
    SwarmObject *swarmObject;
    TentacleIK *tentacle;
protected:
    std::vector<ProjectileRay *> fixedLasers;
    std::vector<EnemyDialog *> dialogs;
public:
    EnemyGhost();

    ~EnemyGhost() override;

    void resolveCollision(Collisionable *withObject) override;

    virtual void onStart();

    void onUpdate() override;

    void integrate() override;

    void remove() override;

    virtual void shoot(Object3D *target);

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


    void takeDamage(float damageTaken);

    void updateEmitterParticles();

    void initLight();

    void updateLight();

    void setSwarmObject(SwarmObject *o);

    void die();

    std::vector<EnemyDialog *> &getDialogs();

    void tryShoot();
};


#endif //BRAKEDA3D_ENEMYGHOST_H
