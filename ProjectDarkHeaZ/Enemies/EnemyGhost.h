
#ifndef BRAKEDA3D_ENEMYGHOST_H
#define BRAKEDA3D_ENEMYGHOST_H


#include "../../src/Collisions/Collisionable.h"
#include "Enemy.h"
#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/Physics/Mesh3DAnimatedGhost.h"
#include "../Weapons/AmmoProjectileBodyEmitter.h"
#include "../../include/Physics/RayCollisionable.h"
#include "../Items/EnemyDialog.h"
#include "../Weapons/ProjectileRay.h"
#include "../../include/Behaviors/RotatableToTarget.h"
#include "../../include/Objects/Swarm.h"
#include "../Items/ExplotableObject.h"

class EnemyGhost : public Mesh3DAnimatedGhost, public Enemy, public LivingObject, public RotatableToTarget, public ExplotableObject {
private:
    Counter counterStuck;

    ParticleEmitter *particleEmitter;
    Vertex3D particlesEngineOffset;

protected:
    std::vector<ProjectileRay *> fixedLasers;
    std::vector<ProjectileRay *> fixedLasersHandled;
    std::vector<EnemyDialog *> dialogs;
    float projectileStartOffsetPosition;
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

    Object3D *getTarget();

    void handleDie();

    void onDrawHostBuffer() override;

    void takeDamage(float damageTaken);

    void initLight();

    void updateLight();

    void die();

    std::vector<EnemyDialog *> &getDialogs();

    void drawImGuiProperties() override;

    void updateEmitters() override;

    void addHandledFixedRay(ProjectileRay *p);
};


#endif //BRAKEDA3D_ENEMYGHOST_H
