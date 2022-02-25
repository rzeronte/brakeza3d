
#ifndef BRAKEDA3D_WEAPONTYPE_H
#define BRAKEDA3D_WEAPONTYPE_H

#include <SDL2/SDL_mixer.h>
#include "../2D/WeaponAnimation.h"
#include "../Objects/SpriteDirectional3D.h"
#include "../Objects/Sprite3D.h"
#include "AmmoType.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Mesh3DAnimatedCollection.h"

#define WEAPON_MAX_ANIMATIONS 10

class WeaponType {
public:
    WeaponType();

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

    Counter *counterCadence;

    int speed;

    // sounds
    std::string fireSound;
    std::string soundEmptyLabel;

    // mesh for
    Mesh3D *model;

    SDL_Surface *iconHUD;

    bool sniper;
    bool sniperEnabled = false;
    SDL_Surface *sniperHUD;

    void onUpdate();

    bool isAvailable() const;

    void setAvailable(bool available);

    void setSpeed(float speed);

    int getSpeed() const;

    float getDamage() const;

    void setDamage(float damage);

    float getDamageRadius() const;

    void setDamageRadius(float damageRadius);

    void loadIconHUD(const std::string& file);

    void loadSniperHUD(const std::string& file);

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

    void shoot();

    void reload();

    Mesh3D *getModel() const;

    const std::string &getSoundEmptyLabel() const;

    void setSoundEmptyLabel(const std::string &soundEmptyLabel);

    void setSoundFire(const std::string &label);

    const std::string &getSoundFire() const;

    const std::string &getLabel() const;
};


#endif //BRAKEDA3D_WEAPONTYPE_H
