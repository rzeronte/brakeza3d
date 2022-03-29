
#ifndef BRAKEDA3D_WEAPON_H
#define BRAKEDA3D_WEAPON_H

#include <SDL2/SDL_mixer.h>
#include "../Objects/SpriteDirectional3D.h"
#include "../Objects/Sprite3D.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Mesh3DAnimatedCollection.h"

enum WeaponTypes {
    WEAPON_PROJECTILE = 0,
    WEAPON_INSTANT = 1,
    WEAPON_SMART_PROJECTILE = 2,
};

class Weapon {
public:
    Weapon(const std::string& label);
    bool available;
    int status;
    int type;

    std::string label;

    Mesh3DBody *modelProjectile;

    int ammoAmount;
    int startAmmoAmount;

    Counter *counterStopDuration;
    Counter *counterStopEvery;
    bool stop;
    float stopDuration;
    float stopEvery;

    float damage;
    float damageRadius;
    float accuracy;
    int dispersion;

    float cadenceTime;
    Counter *counterCadence;

    int speed;

    // sounds
    std::string fireSound;
    std::string soundEmptyLabel;

    Image *icon;

    Mesh3D *model;

    void onUpdate();

    bool isAvailable() const;

    void setAvailable(bool available);

    void setSpeed(float speed);

    int getSpeed() const;

    float getDamage() const;

    void setDamage(float damage);

    float getDamageRadius() const;

    void setDamageRadius(float damageRadius);

    float getAccuracy() const;

    void setAccuracy(float accuracy);

    int getDispersion() const;

    void setDispersion(float dispersion);

    void shootProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, int collisionMask);
    void shootSmartProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, int collisionMask, Object3D *target);
    void shootInstant(Vertex3D from,  Object3D *to);

    void reload();

    Mesh3D *getModel() const;

    const std::string &getSoundEmptyLabel() const;

    void setSoundEmptyLabel(const std::string &soundEmptyLabel);

    void setSoundFire(const std::string &label);

    const std::string &getSoundFire() const;

    const std::string &getLabel() const;

    float getCadenceTime() const;

    void setCadenceTime(float cadenceTime);

    void setIconImage(std::string file);

    Image *getIcon() const;

    int getAmmoAmount() const;

    void setAmmoAmount(int ammoAmount);

    void addAmount(int amount);

    Mesh3DBody *getModelProjectile();

    int getType() const;

    void setType(int type);

    int getStatus() const;

    void setStatus(int status);

    int getStartAmmoAmount() const;

    void setStartAmmoAmount(int startAmmoAmount);

    bool isStop() const;

    void setStop(bool stop);

    float getStopDuration() const;

    void setStopDuration(float stopDuration);

    float getStopEvery() const;

    void setStopEvery(float stopEverySeconds);
};


#endif //BRAKEDA3D_WEAPON_H
