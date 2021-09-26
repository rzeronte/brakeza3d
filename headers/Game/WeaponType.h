
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

    WeaponType(std::string label);

    bool available;
    int status = EngineSetup::WeaponsActions::IDLE;

    int index; // For related with Enum in Setup

    std::string label;
    std::string classname;  // bsp quake map reference

    AmmoType *ammo;

    float damage;
    float damageRadius;

    float accuracy;

    bool firing = false;

    int dispersion = 0;

    Counter *counterCadence;

    int speed = 500;

    int numAnimations = 0;
    int currentAnimationIndex = 0;

    // Animations for this weapon
    Mesh3DAnimatedCollection *weaponAnimations;

    // sounds
    std::vector<std::string> weaponSounds;
    std::string soundEmptyLabel;

    // mesh for
    Mesh3D *model;

    SDL_Surface *iconHUD;

    bool sniper;
    bool sniperEnabled = false;
    SDL_Surface *sniperHUD;

    void addAnimation(std::string label, std::string model, float scale, bool stopEnd);

    Mesh3DAnimated *getCurrentWeaponAnimation();

    void onUpdate();

    void setWeaponAnimation(int);

    bool isAvailable() const;

    void setAvailable(bool available);

    void setSpeed(float speed);

    int getSpeed() const;

    float getDamage();

    void setDamage(float damage);

    float getDamageRadius() const;

    void setDamageRadius(float damageRadius);

    void loadIconHUD(std::string file);

    void loadSniperHUD(std::string file);

    float getAccuracy() const;

    void setAccuracy(float accuracy);

    int getDispersion() const;

    void setDispersion(float dispersion);

    bool isFiring() const;

    void setFiring(bool firing);

    bool isSniper() const;

    void setSniper(bool sniper);

    bool isSniperEnabled() const;

    void setSniperEnabled(bool sniperEnabled);

    const std::string &getClassname() const;

    void setClassname(const std::string &classname);

    int getIndex() const;

    void setIndex(int index);

    AmmoType *getAmmoType() const;

    void setAmmoType(AmmoType *ammo);

    void shoot();

    void reload();

    Mesh3D *getModel();

    Mesh3DAnimatedCollection *getWeaponAnimations() const;

    void setWeaponAnimations(Mesh3DAnimatedCollection *weaponAnimations);

    std::vector<std::string> &getWeaponSounds();

    void setWeaponSounds(const std::vector<std::string> &weaponSounds);

    const std::string &getSoundEmptyLabel() const;

    void setSoundEmptyLabel(const std::string &soundEmptyLabel);

};


#endif //BRAKEDA3D_WEAPONTYPE_H
