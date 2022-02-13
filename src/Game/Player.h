
#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../../include/Objects/Object3D.h"
#include "../../include/Misc/Counter.h"
#include "../../include/Objects/Mesh3D.h"

#define INITIAL_STAMINA 100
#define INITIAL_LIVES 10
#define INITIAL_POWER 1
#define INITIAL_FRICTION 0.5
#define INITIAL_MAX_VELOCITY 10

typedef enum {
    LIVE, DEAD, GAMEOVER
} PlayerState;

class Player : public Mesh3D{
private:

    float stamina;
    bool dead;
    bool stopped;
    int lives;
    Vertex3D velocity;
    M3 velocityRotation;

public:
    PlayerState state;

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

    void onUpdate() override;

    Vertex3D getVelocity();
    void setVelocity(Vertex3D v);

    float power;
    float friction;
    float maxVelocity;

    void applyFriction();
};


#endif //BRAKEZA3D_PLAYER_H
