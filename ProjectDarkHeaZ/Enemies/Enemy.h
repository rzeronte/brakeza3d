
#ifndef BRAKEDA3D_ENEMY_H
#define BRAKEDA3D_ENEMY_H

#include <vector>
#include "../../include/Objects/Vertex3D.h"
#include "../../include/Physics/Body.h"
#include "../../include/Misc/Counter.h"
#include "../../include/Render/M3.h"
#include "../Weapons/Weapon.h"
#include "../Items/LivingObject.h"
#include "../Weapons/AmmoProjectileBodyEmitter.h"

typedef enum {
    ENEMY_STATE_STOP, ENEMY_STATE_DIE
} EnemyState;

class Enemy {
protected:
    LightPoint3D *light;
    Counter counterLight;

    std::string name;
    bool stuck;
    EnemyState state;
    Weapon *weapon;
    Image *avatar;
    Image *avatarHud;
    float range;
    int soundChannel;
    bool rewards;
    Vertex3D lightPositionOffset;
    std::vector<AmmoProjectileBodyEmitter*> projectileEmitters;

public:

    Enemy();

    virtual ~Enemy();

    void setState(EnemyState value);

    void setWeapon(Weapon *weaponType);

    void setSoundChannel(int soundChannel);

    void setRewards(bool value);

    void setStuck(bool value);

    [[nodiscard]] int getSoundChannel() const;

    [[nodiscard]] bool isRewards() const;

    [[nodiscard]] bool isStuck() const;

    [[nodiscard]] Weapon *getWeapon() const;

    [[nodiscard]] EnemyState getState() const;

    void setAvatar(Image *avatar);

    Image *getAvatar();

    [[nodiscard]] const std::string &getName() const;

    void setName(const std::string &name);

    [[nodiscard]] Image *getAvatarHud() const;

    void setAvatarHud(Image *avatarHud);

    [[nodiscard]] LightPoint3D *getLight() const;

    std::vector<AmmoProjectileBodyEmitter *> *getProjectileEmitters();

    void addProjectileEmitter(AmmoProjectileBodyEmitter *emitter);

    virtual void updateEmitters();

    void setEmittersEnabled(bool value);
};


#endif //BRAKEDA3D_ENEMY_H
