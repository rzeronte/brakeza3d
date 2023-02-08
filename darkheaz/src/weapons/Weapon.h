
#ifndef BRAKEDA3D_WEAPON_H
#define BRAKEDA3D_WEAPON_H

#include <SDL2/SDL_mixer.h>
#include "../../../include/Objects/SpriteDirectional3D.h"
#include "../../../include/Objects/Sprite3D.h"
#include "../../../include/Objects/Mesh3D.h"
#include "../../../include/Objects/Mesh3DAnimatedCollection.h"
#include "../shaders/ShaderLaser.h"

enum WeaponTypes {
    WEAPON_PROJECTILE = 0,
    WEAPON_LASER_PROJECTILE = 1,
    WEAPON_LASER_RAY = 2,
    WEAPON_BOMB = 3,
    SHOCK = 4,
};

class Weapon {
public:
    Weapon(const std::string& label);

    bool available;
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

    void onUpdate();

    bool isAvailable() const;

    void setAvailable(bool available);

    void setSpeed(int speed);

    int getSpeed() const;

    float getDamage() const;

    void setDamage(float value);

    float getDamageRadius() const;

    void setDamageRadius(float damageRadius);

    float getAccuracy() const;

    void setAccuracy(float value);

    int getDispersion() const;

    void setDispersion(int value);

    void shootProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, M3 rotation, float intensity, int collisionMask, bool sound);
    void shootSmartProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, M3 rotation, float intensity, int collisionMask, Object3D *target, bool sound);
    void shootLaserProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, float intensity, bool sound, Color color, int filterGroup, int filterMask);

    [[nodiscard]] Mesh3D *getModel() const;

    [[nodiscard]] const std::string &getLabel() const;

    void setCadenceTime(float value);

    void setIconImage(std::string file);

    Image *getIcon() const;

    int getAmmoAmount() const;

    void setAmmoAmount(int value);

    void addAmount(int amount);

    Mesh3D *getModelProjectile();

    int getType() const;

    void setType(int type);

    int getStatus() const;

    void setStatus(int status);

    int getStartAmmoAmount() const;

    void setStartAmmoAmount(int value);

    bool isStop() const;

    void setStop(bool value);

    float getStopDuration() const;

    void setStopDuration(float value);

    float getStopEvery() const;

    void setStopEvery(float stopEverySeconds);

    int getSoundChannel() const;

    void setSoundChannel(int value);

    void stopSoundChannel() const;

    virtual ~Weapon();

    void shootBomb(Object3D *parent, Vertex3D position);

    void shootLaserRay(ShaderLaser *shaderLaser, float intensity);
};


#endif //BRAKEDA3D_WEAPON_H
