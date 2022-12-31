#ifndef BRAKEDA3D_AMMOPROJECTILEBODYEMISSOR_H
#define BRAKEDA3D_AMMOPROJECTILEBODYEMISSOR_H


#include "../../../include/Objects/Object3D.h"
#include "AmmoProjectileBody.h"

enum ProjectileBodyEmmissorType {
    UNIQUE_PROJECTILE = 0,
    CIRCLE_PROJECTILE = 1
};

class AmmoProjectileBodyEmissor: public Object3D {
    Counter counter;
    float step;
    bool active;
    Weapon *weaponType;

    Counter *counterStopDuration;
    Counter *counterStopEvery;
    bool stop;
    float stopDuration;
    float stopEvery;
    ProjectileBodyEmmissorType type;

public:
    AmmoProjectileBodyEmissor(ProjectileBodyEmmissorType type, float step, Weapon *weaponType);

    virtual ~AmmoProjectileBodyEmissor();

    const Counter &getCounter() const;

    void setCounter(const Counter &counter);

    bool isActive() const;

    void setActive(bool active);

    void onUpdate() override;

    Weapon *getWeapon() const;

    void setWeapon(Weapon *weapon);

    void addProjectile();

    bool isStop() const;

    void setStop(bool stop);

    float getStopDuration() const;

    void setStopDuration(float stopDuration);

    float getStopEvery() const;

    void setStopEvery(float stopEverySeconds);

    float getStep() const;

    void setStep(float step);

    ProjectileBodyEmmissorType getType() const;

    void setType(ProjectileBodyEmmissorType type);

    void launchUniqueProjectile();

    void launchCircleProjectiles();
};


#endif //BRAKEDA3D_AMMOPROJECTILEBODYEMISSOR_H
