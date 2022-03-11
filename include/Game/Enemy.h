
#ifndef BRAKEDA3D_ENEMY_H
#define BRAKEDA3D_ENEMY_H


#include <vector>
#include "../Objects/Vertex3D.h"
#include "../Physics/Body.h"
#include "../Misc/Counter.h"
#include "../Render/M3.h"


typedef enum {
    ENEMY_STATE_STOP, ENEMY_STATE_FOLLOW, ENEMY_STATE_ATTACK, ENEMY_STATE_INJURIED, ENEMY_STATE_DIE
} EnemyState;

class Enemy {
public:
    EnemyState state;

    float startStamina;
    float stamina;

    float cadence;          // cadence management
    Counter *counterCadence;

    float range;
    float speed;

    Enemy();

    void takeDamage(float damageTaken);

    float getRange() const;

    void setRange(float range);

    float getSpeed() const;

    void setSpeed(float speed);

    float getCadence() const;

    void setCadence(float cadence);

    void evalStatusMachine();

    EnemyState getState() const;

    void setState(EnemyState state);

    float getStamina() const;
};


#endif //BRAKEDA3D_ENEMY_H
