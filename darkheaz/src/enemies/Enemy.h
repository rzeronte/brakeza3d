
#ifndef BRAKEDA3D_ENEMY_H
#define BRAKEDA3D_ENEMY_H

#include <vector>
#include "../../../include/Objects/Vertex3D.h"
#include "../../../include/Physics/Body.h"
#include "../../../include/Misc/Counter.h"
#include "../../../include/Render/M3.h"
#include "../weapons/Weapon.h"

typedef enum {
    ENEMY_STATE_STOP, ENEMY_STATE_DIE
} EnemyState;

class Enemy {
protected:
    bool stuck;
    EnemyState state;
    Weapon *weapon;
    Image *avatar;
    float startStamina;
    float stamina;
    float range;
    int soundChannel;
    bool rewards;
public:

    Enemy();

    virtual ~Enemy();

    void takeDamage(float damageTaken);

    void setState(EnemyState value);

    void setWeapon(Weapon *weaponType);

    void setStamina(float stamina);

    void setStartStamina(float startStamina);

    void setSoundChannel(int soundChannel);

    void setRewards(bool value);

    void setStuck(bool value);

    [[nodiscard]] int getSoundChannel() const;

    [[nodiscard]] bool isRewards() const;

    [[nodiscard]] bool isStuck() const;

    [[nodiscard]] float getStartStamina() const;

    [[nodiscard]] Weapon *getWeapon() const;

    [[nodiscard]] float getStamina() const;

    [[nodiscard]] EnemyState getState() const;

    void setAvatar(Image *avatar);

    Image *getAvatar();
};


#endif //BRAKEDA3D_ENEMY_H
