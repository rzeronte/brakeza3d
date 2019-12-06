
#ifndef BRAKEDA3D_WEAPONTYPE_H
#define BRAKEDA3D_WEAPONTYPE_H

#include <SDL_mixer.h>
#include "WeaponAnimation.h"
#include "../Objects/SpriteDirectional3D.h"
#include "../Objects/Sprite3D.h"

#define WEAPON_MAX_ANIMATIONS 10

class WeaponType {
public:
    WeaponType(std::string label);

    int status = EngineSetup::getInstance()->WeaponsActions::WALKING;

    std::string label;

    int ammo = 50;

    float projectileWidth;
    float projectileHeight;

    int damage;

    float accuracy;

    bool firing = false;
    std::vector<Counter>    fireCounters;
    std::vector<Mix_Chunk*> fireSounds;

    bool keyDownHandle;
    bool keyUpHandle;

    int keyDownAnimationStatus;
    int keyUpAnimationStatus;

    int dispersion = 0;

    SpriteDirectional3D *projectileTemplate;
    Sprite3D *markTemplate;

    Mix_Chunk *soundFire;
    Mix_Chunk *soundMark;

    int speed = 500;

    int numAnimations = 0;
    int currentAnimationIndex = 0;
    WeaponAnimation *animations[WEAPON_MAX_ANIMATIONS];

    SDL_Surface *iconHUD;

    bool sniper;

    void addAnimation(std::string, int frames, int fps, int offsetX, int offsetY, bool right, bool stopEnd, int next, bool looping);

    WeaponAnimation *getCurrentWeaponAnimation();

    WeaponType();

    void onUpdate();

    void setWeaponAnimation(int);

    void setSpeed(float speed);
    int  getSpeed() const;

    void makeProjectileTemplate();
    SpriteDirectional3D* getProjectileTemplate();

    void setProjectileSize(float w, float h);

    int  getDamage();
    void setDamage(int damage);

    Sprite3D *getMarkTemplate();
    void setupMarkTemplate(std::string path, int numFrames, int fps, float w, float h);

    void loadFireSound(std::string file);
    void loadMarkSound(std::string file);
    void loadIconHUD(std::string file);

    int  getAmmo() const;
    void setAmmo(int ammo);

    float getAccuracy() const;
    void  setAccuracy(float accuracy);

    int  getDispersion() const;
    void setDispersion(float dispersion);

    bool isFiring() const;
    void setFiring(bool firing);

    bool isKeyDownHandle() const;
    void setKeyDownHandle(bool keyDownHandle);

    bool isKeyUpHandle() const;
    void setKeyUpHandle(bool keyUpHandle);

    int  getKeyDownAnimationStatus() const;
    void setKeyDownAnimationStatus(int keyDownAnimationStatus);

    int  getKeyUpAnimationStatus() const;
    void setKeyUpAnimationStatus(int keyUpAnimationStatus);

    bool isSniper() const;
    void setSniper(bool sniper);
};


#endif //BRAKEDA3D_WEAPONTYPE_H
