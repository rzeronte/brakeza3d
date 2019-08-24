
#ifndef BRAKEDA3D_ENEMY_H
#define BRAKEDA3D_ENEMY_H


class Enemy {
public:
    float startStamina = 100;
    float stamina = 0;
    bool dead = 0;

    Enemy();

    void damage(float dmg);
    bool isHeavyDamage(float dmg);
};


#endif //BRAKEDA3D_ENEMY_H
