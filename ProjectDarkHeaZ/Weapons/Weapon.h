
#ifndef BRAKEDA3D_WEAPON_H
#define BRAKEDA3D_WEAPON_H

#include <SDL2/SDL_mixer.h>
#include "../../include/Objects/SpriteDirectional3D.h"
#include "../../include/Objects/Sprite3D.h"
#include "../../include/Objects/Mesh3D.h"
#include "../../include/Objects/Mesh3DAnimatedCollection.h"
#include "../FX/RayLight.h"

enum WeaponTypes {
    WEAPON_PROJECTILE = 0,
    WEAPON_LASER = 1,
    WEAPON_RAYLIGHT = 2,
    WEAPON_BOMB = 3,
    WEAPON_REFLECTION = 4,
    WEAPON_SHIELD = 5,
    WEAPON_SHOCK = 6,
};

static std::map<std::string, WeaponTypes> WeaponTypesMapping = {
    {"projectile", WeaponTypes::WEAPON_PROJECTILE},
    {"laser", WeaponTypes::WEAPON_LASER},
    {"raylight", WeaponTypes::WEAPON_RAYLIGHT},
    {"bomb", WeaponTypes::WEAPON_BOMB},
    {"reflection", WeaponTypes::WEAPON_REFLECTION},
    {"shield", WeaponTypes::WEAPON_SHIELD},
    {"shock", WeaponTypes::WEAPON_SHOCK},
};

struct WeaponShootAttributes {
    WeaponShootAttributes(
        Vertex3D direction,
        float startOffset,
        int filterGroup,
        int filterMask
    )
    :
    direction(direction),
    startOffset(startOffset),
    filterGroup(filterGroup),
    filterMask(filterMask)
    {}
    Vertex3D direction = Vertex3D(0, 0, 0);
    float startOffset = 0;
    int filterGroup = 0;
    int filterMask = 0;
};

struct WeaponAttributes {
    Object3D *parent;
    const std::string& name;
    const std::string& weaponModel;
    const std::string& projectileModel;
    const std::string& icon;
            Color projectileColor;
    bool projectileFlatTexture;
    bool projectileEnableLights;
    int ammoAmount;
    int startAmmoAmount;
    float damage;
    float speed;
    int dispersion;
    float accuracy;
    float cadenceTime;
    bool stop;
    float stopEver;
    float stopDuration;
    bool available;
    bool selectable;
    RayLight *rayLight;
};

class Weapon {
private:

    bool available;
    bool selectable;
    int soundChannel;

    std::string label;

    int startAmmoAmount;

    Counter counterStopEvery;
    bool stop;
    float stopDuration;
    float stopEvery;

    float damage;
    float damageRadius;
    int dispersion;

    float speed;

    Image *icon;
    Mesh3D *model;
    Mesh3D *modelProjectile;
protected:
    int status;
    RayLight *rayLight;
    Object3D *parent;
    Counter *counterCadence;
    float cadenceTime;
    Counter counterStopDuration;
    float accuracy;
    int ammoAmount;
public:
    Weapon(WeaponAttributes attributes);

    virtual void onUpdate();

    void setAvailable(bool available);

    void setSpeed(float speed);

    void setDamage(float value);

    void setDamageRadius(float damageRadius);

    void setAccuracy(float value);

    void setDispersion(int value);

    virtual bool shoot(WeaponShootAttributes attributes);

    void setCadenceTime(float value);

    void setIconImage(const std::string& file);

    void setAmmoAmount(int value);

    void addAmount(int amount);

    Mesh3D *getModelProjectile();

    void setStatus(int status);

    void setStartAmmoAmount(int value);

    void setStop(bool value);

    void setStopDuration(float value);

    void setStopEvery(float stopEverySeconds);

    void setSoundChannel(int value);

    void stopSoundChannel() const;

    virtual ~Weapon();

    void setLabel(const std::string &value);

    [[nodiscard]] Counter *getCounterCadence() const;

    [[nodiscard]] int getSoundChannel() const;

    [[nodiscard]] float getStopEvery() const;

    [[nodiscard]] float getStopDuration() const;

    [[nodiscard]] bool isStop() const;

    [[nodiscard]] int getStartAmmoAmount() const;

    [[nodiscard]] int getStatus() const;

    [[nodiscard]] bool isAvailable() const;

    [[nodiscard]] float getSpeed() const;

    [[nodiscard]] float getDamage() const;

    [[nodiscard]] float getDamageRadius() const;

    [[nodiscard]] float getAccuracy() const;

    [[nodiscard]] int getDispersion() const;

    [[nodiscard]] Mesh3D *getModel() const;

    [[nodiscard]] const std::string &getLabel() const;

    [[nodiscard]] Image *getIcon() const;

    [[nodiscard]] int getAmmoAmount() const;


    [[nodiscard]] bool isSelectable() const;

    void drawImGuiProperties();
};


#endif //BRAKEDA3D_WEAPON_H
