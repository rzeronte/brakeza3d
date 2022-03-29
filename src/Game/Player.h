
#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../../include/Objects/Object3D.h"
#include "../../include/Misc/Counter.h"
#include "../../include/Particles/ParticleEmissor.h"
#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/Game/Weapon.h"
#include "ShaderShield.h"

#define INITIAL_STAMINA 100
#define INITIAL_ENERGY 100
#define INITIAL_RECOVER_ENERGY 0.01f
#define INITIAL_LIVES 10
#define INITIAL_POWER 100
#define INITIAL_POWERDASH 6000
#define INITIAL_FRICTION 5
#define INITIAL_MAX_VELOCITY 25

typedef enum {
    EMPTY, LIVE, DEAD, GETTING_DAMAGE
} PlayerState;

enum WeaponStatus {
    NONE = -1,
    PRESSED = 1,
    SUSTAINED = 2,
    RELEASED = 3,
    RELOAD = 4,
};

class Player : public Mesh3DGhost {
private:

    float stamina;
    float startStamina;
    float energy;
    float startEnergy;
    float recoverEnergySpeed;

    int lives;
    Vertex3D velocity;
    Weapon *weaponType;
    Counter *counterDamage;
    int killsCounter;
    int levelsCompletedCounter;
    bool shieldEnabled;

public:
    ParticleEmissor *engineParticles;
    Vertex3D engineParticlesPositionOffset;

    LightPoint3D *light;
    Vertex3D lightPositionOffset;

    PlayerState state;

    ShaderShield *shield;
    std::vector<Weapon *> weaponTypes;

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

    float dashPower;
    float power;
    float friction;
    float maxVelocity;

    void applyFriction();

    void updateEngineParticles();

    void updateLight();

    void resolveCollision(Collisionable *with) override;

    float autoRotationSelectedObjectSpeed;

    void setState(PlayerState state);

    Weapon *getWeapon() const;

    void setWeaponType(Weapon *weaponType);

    void updateWeaponType();

    void createWeapon(const std::string &label);

    void addWeapon(Weapon *weaponType);

    Weapon *getWeaponTypeByLabel(const std::string& label);

    void setWeaponTypeByIndex(int i);

    void setAutoRotationSelectedObjectSpeed(float autoRotationSelectedObjectSpeed);

    void startBlinkShaderForPlayer();

    void stopBlinkForPlayer();

    int getKillsCounter() const;

    void setKillsCounter(int killsCounter);

    void increaseKills();

    void increaseLevelsCompleted();

    int getLevelCompletedCounter() const;

    void setLevelCompletedCounter(int levelCounter);

    const std::vector<Weapon *> &getWeaponTypes() const;

    bool isShieldEnabled() const;

    void setShieldEnabled(bool shieldEnabled);

    float getEnergy() const;

    void setEnergy(float energy);

    int getStartStamina() const;

    void setStartStamina(int startStamina);

    float getStartEnergy() const;

    void setStartEnergy(float startEnergy);

    float getRecoverEnergySpeed() const;

    void setRecoverEnergySpeed(float recoverEnergySpeed);

    void startPlayerBlink();
};


#endif //BRAKEZA3D_PLAYER_H
