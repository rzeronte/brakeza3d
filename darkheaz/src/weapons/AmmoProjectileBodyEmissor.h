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
    Color color;

public:
    AmmoProjectileBodyEmissor(ProjectileBodyEmmissorType type, float step, Weapon *weaponType);

    ~AmmoProjectileBodyEmissor() override;

    [[nodiscard]] const Counter &getCounter() const;

    void setCounter(const Counter &counter);

    [[nodiscard]] bool isActive() const;

    void setActive(bool active);

    void onUpdate() override;

    [[nodiscard]] Weapon *getWeapon() const;

    void setWeapon(Weapon *weapon);

    void addProjectile();

    [[nodiscard]] bool isStop() const;

    void setStop(bool stop);

    [[nodiscard]] float getStopDuration() const;

    void setStopDuration(float stopDuration);

    [[nodiscard]] float getStopEvery() const;

    void setStopEvery(float stopEverySeconds);

    [[nodiscard]] float getStep() const;

    void setStep(float step);

    [[nodiscard]] ProjectileBodyEmmissorType getType() const;

    void setType(ProjectileBodyEmmissorType type);

    void launchUniqueProjectile();

    void launchCircleProjectiles();

    [[nodiscard]] const Color &getColor() const;

    void setColor(const Color &color);
};


#endif //BRAKEDA3D_AMMOPROJECTILEBODYEMISSOR_H
