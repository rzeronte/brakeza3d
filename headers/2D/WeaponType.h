
#ifndef BRAKEDA3D_WEAPONTYPE_H
#define BRAKEDA3D_WEAPONTYPE_H

#include <SDL_mixer.h>
#include "WeaponAnimation.h"
#include "../Objects/SpriteDirectional3D.h"
#include "../Objects/Sprite3D.h"

#define WEAPON_MAX_ANIMATIONSS 10

class WeaponType {
public:
    WeaponType(std::string label);

    int status = EngineSetup::getInstance()->WeaponsActions::WEAPON_ACTION_WALK;

    std::string label;

    int ammo = 50;

    float projectileWidth;
    float projectileHeight;

    int hitType;

    int damage;

    float accuracy;

    float cadence = 1;
    Timer cadenceTimer;
    float lastTicks;
    float acumulatedTime = 0;

    int dispersion = 0;

    SpriteDirectional3D *projectileTemplate;
    Sprite3D *markTemplate;

    Mix_Chunk *soundFire;
    Mix_Chunk *soundMark;

    int speed = 500;

    int numAnimations = 0;
    int currentAnimationIndex = 0;
    WeaponAnimation *animations[WEAPON_MAX_ANIMATIONSS];

    void addAnimation(std::string, int frames, int fps, int offsetX, int offsetY, bool right);

    WeaponAnimation *getCurrentWeaponAnimation();

    void onUpdate();

    void setWeaponAnimation(int);

    void setCadence(float cadence);
    void setSpeed(float cadence);
    int  getSpeed() const;

    void startAction();
    void endAction();

    void updateCadenceTimer();
    bool isCadenceInProgress();

    int getHitType();
    void setHitType(int hitType);

    void makeProjectileTemplate();
    SpriteDirectional3D* getProjectileTemplate();

    void setProjectileSize(float w, float h);

    int getDamage();
    void setDamage(int damage);

    Sprite3D *getMarkTemplate();
    void setupMarkTemplate(std::string path, int numFrames, int fps, float w, float h);

    void loadFireSound(std::string file);
    void loadMarkSound(std::string file);

    int  getAmmo() const;
    void setAmmo(int ammo);

    float getAccuracy() const;
    void  setAccuracy(float accuracy);

    int  getDispersion() const;
    void setDispersion(float dispersion);
};


#endif //BRAKEDA3D_WEAPONTYPE_H
