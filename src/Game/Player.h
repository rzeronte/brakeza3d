
#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../../include/Objects/Object3D.h"
#include "../../include/Misc/Counter.h"
#include "../../include/Particles/ParticleEmissor.h"
#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/Game/WeaponType.h"

#define INITIAL_STAMINA 100
#define INITIAL_LIVES 10
#define INITIAL_POWER 100
#define INITIAL_FRICTION 5
#define INITIAL_MAX_VELOCITY 25

typedef enum {
    LIVE, DEAD
} PlayerState;

enum WeaponStatus {
    NONE = -1,
    IDLE = 0,
    FIRE = 1,
    RELOAD = 2,
};

class Player : public Mesh3DGhost {
private:

    float stamina;
    int lives;
    Vertex3D velocity;
    WeaponType *weaponType;

public:
    ParticleEmissor *engineParticles;
    Vertex3D engineParticlesPositionOffset;

    LightPoint3D *light;
    Vertex3D lightPositionOffset;

    PlayerState state;

    std::vector<WeaponType *> weaponTypes;
    std::vector<AmmoType *> ammoTypes;

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

    void evalStatusMachine();

    void onUpdate() override;

    Vertex3D getVelocity();
    void setVelocity(Vertex3D v);

    float power;
    float friction;
    float maxVelocity;

    void applyFriction();

    void updateEngineParticles();

    void updateLight();

    void resolveCollision(Collisionable *with) override;

    float autoRotationSelectedObjectSpeed;

    void setState(PlayerState state);

    WeaponType *getWeaponType() const;

    void setWeaponType(WeaponType *weaponType);

    void updateWeaponType();

    void addWeaponType(const std::string&);

    WeaponType *getWeaponTypeByLabel(const std::string& label);

    void setWeaponTypeByIndex(int i);

};


#endif //BRAKEZA3D_PLAYER_H
