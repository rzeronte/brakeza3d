
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
    EMPTY, LIVE, DEAD, GETTING_DAMAGE
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
    Counter *counterDamage;
    int killsCounter;
    int levelsCompletedCounter;
public:
    ParticleEmissor *engineParticles;
    Vertex3D engineParticlesPositionOffset;

    LightPoint3D *light;
    Vertex3D lightPositionOffset;

    PlayerState state;

    std::vector<WeaponType *> weaponTypes;

    Player();

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

    void createWeaponType(const std::string &label);

    void addWeaponType(WeaponType *weaponType);

    WeaponType *getWeaponTypeByLabel(const std::string& label);

    void setWeaponTypeByIndex(int i);

    void setAutoRotationSelectedObjectSpeed(float autoRotationSelectedObjectSpeed);

    void startBlinkForPlayer();

    void stopBlinkForPlayer();

    int getKillsCounter() const;

    void setKillsCounter(int killsCounter);

    void increaseKills();

    void increaseLevelsCompleted();

    int getLevelCompletedCounter() const;

    void setLevelCompletedCounter(int levelCounter);
};


#endif //BRAKEZA3D_PLAYER_H
