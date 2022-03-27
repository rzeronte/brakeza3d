
#ifndef BRAKEDA3D_WEAPON_H
#define BRAKEDA3D_WEAPON_H

#include <SDL2/SDL_mixer.h>
#include "../Objects/SpriteDirectional3D.h"
#include "../Objects/Sprite3D.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Mesh3DAnimatedCollection.h"

enum WeaponTypes {
    WEAPON_PROJECTILE = 0,
    WEAPON_SMART_PROJECTILE = 1,
    WEAPON_INSTANT = 2
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

    // mesh for
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

    void shootProjectile(Object3D *parent, Vertex3D position, Vertex3D direction);
    void shootSmartProjectile(Object3D *parent, Vertex3D vertex3D, Vertex3D vertex3D1);
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

};


#endif //BRAKEDA3D_WEAPON_H
