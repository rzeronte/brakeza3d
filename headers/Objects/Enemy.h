
#ifndef BRAKEDA3D_ENEMY_H
#define BRAKEDA3D_ENEMY_H


class Enemy {
public:
    float stamina = 100;
    bool dead = 0;

    void setDamage(float dmg);
};


#endif //BRAKEDA3D_ENEMY_H
