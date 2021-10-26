
#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../../include/Objects/Object3D.h"
#include "../../include/Misc/Counter.h"

typedef enum {
    LIVE, DEAD, GAMEOVER
} PlayerState;

class Player {
private:

    float stamina;
    bool dead;
    bool stooped;
    int lives;
    int defaultLives;

public:
    PlayerState state;

    Counter *counterStep;
    Counter *counterSoundTakeDamage;

    Player();

    void newGame();

    void respawn();

    void shoot();

    void reload();

    void jump();

    void takeDamage(float dmg);

    void getAid(float aid);

    [[nodiscard]] int getStamina() const;

    void setStamina(int stamina);

    void setLives(int lives);

    [[nodiscard]] bool isDead() const;

    void setDead(bool dead);

    void evalStatusMachine();

    void respawnNPCS();

    [[nodiscard]] bool isCrouch() const;

    void setCrouch(bool stooped);
};


#endif //BRAKEZA3D_PLAYER_H
