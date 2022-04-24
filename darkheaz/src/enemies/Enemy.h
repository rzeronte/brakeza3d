
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
public:
    EnemyState state;
    Weapon *weapon;

    float startStamina;
    float stamina;

    float range;
    float speed;

    int soundChannel;

    Enemy();

    void takeDamage(float damageTaken);

    float getRange() const;

    void setRange(float range);

    float getSpeed() const;

    void setSpeed(float speed);

    EnemyState getState() const;

    void setState(EnemyState state);

    float getStamina() const;

    void setWeapon(Weapon *weaponType);

    Weapon *getWeapon() const;

    void setStamina(float stamina);

    float getStartStamina() const;

    void setStartStamina(float startStamina);

    int getSoundChannel() const;

    void setSoundChannel(int soundChannel);

    virtual ~Enemy();
};


#endif //BRAKEDA3D_ENEMY_H
