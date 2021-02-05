
#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../../headers/Objects/Object3D.h"
#include "../../headers/Misc/Counter.h"

typedef enum {LIVE, DEAD, GAMEOVER} PlayerState;

class Player {
private:

    float stamina;
    bool  dead;
    bool  stooped;
    int   lives;
    int   defaultLives;
    float oxygen;

public:
    PlayerState state;

    Counter *counterStep;
    Counter *counterSoundTakeDamage;

    bool tookDamage;

    Player();

    void newGame();
    void respawn();

    void shoot();
    void reload();
    void jump();

    void takeDamage(float dmg);
    void getAid(float aid);

    int getStamina() const;
    void setStamina(int stamina);

    int getLives() const;
    void setLives(int lives);

    bool isDead() const;
    void setDead(bool dead);

    void evalStatusMachine();
    void respawnNPCS();

    bool isCrouch() const;
    void setCrouch(bool stooped);

    float getOxygen() const;
    void  setOxygen(float air);
};


#endif //BRAKEZA3D_PLAYER_H
