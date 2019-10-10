
#ifndef BRAKEDA3D_ENEMY_H
#define BRAKEDA3D_ENEMY_H


#include <vector>
#include "Vertex3D.h"

class Enemy {
public:
    float startStamina = 100;
    float stamina = 0;
    bool dead = false;

    Vertex3D pathfindingStart;
    Vertex3D pathfindingEnd;
    std::vector<Vertex3D> points;

    Enemy();

    void damage(float dmg);
    bool isHeavyDamage(float dmg);


};


#endif //BRAKEDA3D_ENEMY_H
