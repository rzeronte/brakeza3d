
#ifndef BRAKEDA3D_WEAPON_H
#define BRAKEDA3D_WEAPON_H

#include <SDL2/SDL_mixer.h>
#include "../../../include/Objects/SpriteDirectional3D.h"
#include "../../../include/Objects/Sprite3D.h"
#include "../../../include/Objects/Mesh3D.h"
#include "../../../include/Objects/Mesh3DAnimatedCollection.h"
#include "../shaders/RayLight.h"

enum WeaponTypes {
    WEAPON_PROJECTILE = 0,
    WEAPON_LASER_PROJECTILE = 1,
    WEAPON_LASER_RAY = 2,
    WEAPON_BOMB = 3,
    SHOCK = 4,
    WEAPON_HOLOGRAM = 5,
    WEAPON_SHIELD = 6
};

class Weapon {
private:
    bool available;
    bool selectable;
    int status;
    int type;
    int soundChannel;

    std::string label;

    Mesh3D *modelProjectile;

    int ammoAmount;
    int startAmmoAmount;

    Counter counterStopDuration;
    Counter counterStopEvery;
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

    Image *icon;

    Mesh3D *model;
public:
    Weapon(
        const std::string& name,
        const std::string& weaponModel,
        const std::string& projectileModel,
        const std::string& icon,
        Color projectileColor,
        bool projectileFlatTexture,
        bool projectileEnableLights,
        int ammoAmount,
        int startAmmoAmount,
        float damage,
        int speed,
        int dispersion,
        float accuracy,
        float cadenceTime,
        bool stop,
        float stopEver,
        float stopDuration,
        int type,
        bool available,
        bool selectable
    );

    void onUpdate();

    void setAvailable(bool available);

    void setSpeed(int speed);

    void setDamage(float value);

    void setDamageRadius(float damageRadius);

    void setAccuracy(float value);

    void setDispersion(int value);

    void shootProjectile(Object3D *parent, Vertex3D position, Vertex3D offsetPosition, Vertex3D direction, M3 rotation, float intensity, int filterGroup, int filterMask, bool sound, bool allowMirror);

    void shootLaserProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, float intensity, bool sound, Color color, int filterGroup, int filterMask);

    void setCadenceTime(float value);

    void setIconImage(const std::string& file);

    void setAmmoAmount(int value);

    void addAmount(int amount);

    Mesh3D *getModelProjectile();

    void setType(int type);

    void setStatus(int status);

    void setStartAmmoAmount(int value);

    void setStop(bool value);

    void setStopDuration(float value);

    void setStopEvery(float stopEverySeconds);

    void setSoundChannel(int value);

    void stopSoundChannel() const;

    virtual ~Weapon();

    void shootBomb(Object3D *parent, Vertex3D position);

    void shootShield(Object3D *parent, Vertex3D position);

    void shootRayLight(RayLight &rayLight, float intensity);

    void setLabel(const std::string &value);

    [[nodiscard]] Counter *getCounterCadence() const;

    [[nodiscard]] int getSoundChannel() const;

    [[nodiscard]] float getStopEvery() const;

    [[nodiscard]] float getStopDuration() const;

    [[nodiscard]] bool isStop() const;

    [[nodiscard]] int getStartAmmoAmount() const;

    [[nodiscard]] int getStatus() const;

    [[nodiscard]] bool isAvailable() const;

    [[nodiscard]] int getSpeed() const;

    [[nodiscard]] float getDamage() const;

    [[nodiscard]] float getDamageRadius() const;

    [[nodiscard]] float getAccuracy() const;

    [[nodiscard]] int getDispersion() const;

    [[nodiscard]] Mesh3D *getModel() const;

    [[nodiscard]] const std::string &getLabel() const;

    [[nodiscard]] Image *getIcon() const;

    [[nodiscard]] int getAmmoAmount() const;

    [[nodiscard]] int getType() const;

    void shootHologram(Object3D *parent, Vertex3D position);

    bool isSelectable() const;

    void setSelectable(bool selectable);
};


#endif //BRAKEDA3D_WEAPON_H
