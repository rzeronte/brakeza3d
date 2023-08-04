#ifndef BRAKEDA3D_AMMOPROJECTILEBODYEMISSOR_H
#define BRAKEDA3D_AMMOPROJECTILEBODYEMISSOR_H


#include "../../include/Objects/Object3D.h"
#include "AmmoProjectileBody.h"

enum ProjectileBodyEmmitterType {
    UNIQUE_PROJECTILE = 0,
    CIRCLE_PROJECTILE = 1,
    LASER_UNIQUE_PROJECTILE = 2,
    LASER_CIRCLE_PROJECTILE = 3,
};

class AmmoProjectileBodyEmitter: public Object3D {
    Counter counter;

    bool active;
    bool stop;

    Weapon *weaponType;
    Counter counterStopDuration;
    Counter counterStopEvery;

    ProjectileBodyEmmitterType type;
    Color color;

public:
    AmmoProjectileBodyEmitter(
        ProjectileBodyEmmitterType type,
        float step,
        bool stop,
        float stopDuration,
        float stopEvery,
        Weapon *weaponType,
        Color c
    );

    ~AmmoProjectileBodyEmitter() override;

    [[nodiscard]] bool isActive() const;

    void setActive(bool active);

    void onUpdate() override;

    [[nodiscard]] Weapon *getWeapon() const;

    void addProjectile();

    [[nodiscard]] bool isStop() const;

    [[nodiscard]] ProjectileBodyEmmitterType getType() const;

    void launchUniqueProjectile();

    void launchCircleProjectiles();

    void launchUniqueLaser();

    void launchCircleLaser();

    [[nodiscard]] const Color &getColor() const;

    void setColor(const Color &color);
};


#endif //BRAKEDA3D_AMMOPROJECTILEBODYEMISSOR_H
