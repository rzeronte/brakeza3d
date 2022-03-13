
#ifndef BRAKEDA3D_WEAPONTYPE_H
#define BRAKEDA3D_WEAPONTYPE_H

#include <SDL2/SDL_mixer.h>
#include "../2D/WeaponAnimation.h"
#include "../Objects/SpriteDirectional3D.h"
#include "../Objects/Sprite3D.h"
#include "AmmoType.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Mesh3DAnimatedCollection.h"

class WeaponType {
public:
    WeaponType(const std::string& label);
    bool available;
    int status;

    int index; // For related with Enum in Setup

    std::string label;

    AmmoType *ammoType;

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

    // mesh for
    Mesh3D *model;

    bool sniper;
    bool sniperEnabled = false;

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

    bool isSniper() const;

    void setSniper(bool sniper);

    bool isSniperEnabled() const;

    void setSniperEnabled(bool sniperEnabled);

    int getIndex() const;

    void setIndex(int index);

    AmmoType *getAmmoType() const;

    void setAmmoType(AmmoType *ammo);

    void shoot(Object3D *parent, Vertex3D position, Vertex3D direction);

    void reload();

    Mesh3D *getModel() const;

    const std::string &getSoundEmptyLabel() const;

    void setSoundEmptyLabel(const std::string &soundEmptyLabel);

    void setSoundFire(const std::string &label);

    const std::string &getSoundFire() const;

    const std::string &getLabel() const;

    float getCadenceTime() const;

    void setCadenceTime(float cadenceTime);
};


#endif //BRAKEDA3D_WEAPONTYPE_H
