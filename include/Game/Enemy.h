
#ifndef BRAKEDA3D_ENEMY_H
#define BRAKEDA3D_ENEMY_H


#include <vector>
#include "../Objects/Vertex3D.h"
#include "../Physics/Body.h"
#include "../Misc/Counter.h"
#include "../Render/M3.h"
#include "Weapon.h"


typedef enum {
    ENEMY_STATE_STOP, ENEMY_STATE_DIE
} EnemyState;

class Enemy {
public:
    EnemyState state;
    Weapon *weaponType;

    float startStamina;
    float stamina;

    float range;
    float speed;

    Enemy();

    void takeDamage(float damageTaken);

    float getRange() const;

    void setRange(float range);

    float getSpeed() const;

    void setSpeed(float speed);

    EnemyState getState() const;

    void setState(EnemyState state);

    float getStamina() const;

    void setWeaponType(Weapon *weaponType);

    void shoot(Object3D *parent, Vertex3D direction, Vertex3D projectilePosition);

    Weapon *getWeaponType() const;

    void setStamina(float stamina);

    float getStartStamina() const;

    void setStartStamina(float startStamina);
};


#endif //BRAKEDA3D_ENEMY_H
