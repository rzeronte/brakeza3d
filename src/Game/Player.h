
#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../../headers/Objects/Object3D.h"
typedef enum {LIVE, DEAD, GAMEOVER} PlayerState;

class Player {
private:

    float stamina;
    int lives;
    int defaultLives;
    bool dead;

public:
    PlayerState state;

    Player();

    void newGame();
    void respawn();

    void shoot();

    void takeDamage(float dmg);

    int getStamina() const;
    void setStamina(int stamina);

    int getLives() const;
    void setLives(int lives);

    bool isDead() const;
    void setDead(bool dead);

    void evalStatusMachine();
};


#endif //BRAKEZA3D_PLAYER_H
