
#ifndef BRAKEDA3D_ENEMY_H
#define BRAKEDA3D_ENEMY_H


#include <vector>
#include "Vertex3D.h"
#include "../Physics/Body.h"

typedef enum {ENEMY_STATE_STOP, ENEMY_STATE_FOLLOW, ENEMY_STATE_ATTACK, ENEMY_STATE_DIE} EnemyState;
typedef enum {NONE, PATHFOUND, FIRE} EnemyStateCommands;

class Enemy {
public:

    EnemyState state;
    static EnemyStateCommands stateCommands;

    std::string classname;  // For BSP classname match

    float startStamina;
    float stamina;

    bool dead;

    float damage;   // Damage from enemy to others
    float range;
    float speed;

    std::vector<Vertex3D> points;

    Enemy();

    void takeDamage(float damageTaken);
    bool isTakeHeavyDamage(float damageTaken);

    float getDamage() const;
    void setDamage(float damage);

    float getRange() const;
    void setRange(float range);

    float getSpeed() const;
    void setSpeed(float speed);

    const std::string &getClassname() const;
    void setClassname(const std::string &classname);

    bool isDead() const;
    void setDead(bool dead);

    void evalStatusMachine(Object3D *object, bool raycastResult, Body *);

    void doFollowPathfinding(Object3D *object, Body *body, bool raycastResult);
};


#endif //BRAKEDA3D_ENEMY_H
