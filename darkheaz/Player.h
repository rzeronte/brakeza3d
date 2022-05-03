
#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../include/Objects/Object3D.h"
#include "../include/Misc/Counter.h"
#include "../include/Particles/ParticleEmissor.h"
#include "../include/Physics/Mesh3DGhost.h"
#include "src/weapons/Weapon.h"
#include "ShaderShockWave.h"
#include "GravitationalShield.h"

#define INITIAL_STAMINA 100
#define INITIAL_ENERGY 100
#define INITIAL_RECOVER_ENERGY 0.05f
#define INITIAL_LIVES 10
#define INITIAL_POWER 100
#define INITIAL_POWERDASH 6000
#define INITIAL_FRICTION 5
#define INITIAL_MAX_VELOCITY 25
#define MAX_GRAVITATIONAL_SHIELDS 3

typedef enum {
    EMPTY = -1,
    LIVE = 0,
    DEAD = 1,
    GETTING_DAMAGE = 2
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
    Weapon *weapon;
    Counter *counterDamageBlink;
    ShaderBlink *blink;
    ShaderShockWave *shockWave;

    int killsCounter;
    int levelsCompletedCounter;
    bool energyShieldEnabled;
    int gravityShieldsNumber;

    bool allowGravitationalShields;
    bool allowEnergyShield;
public:
    ParticleEmissor *engineParticles;
    Vertex3D engineParticlesPositionOffset;

    LightPoint3D *light;
    Vertex3D lightPositionOffset;

    PlayerState state;

    std::vector<Weapon *> weaponTypes;
    int currentWeaponIndex;

    Mesh3D *shieldModel;

    Player();

    void respawn();

    void shoot();

    void makeGravitationalShield();

    void takeDamage(float dmg);

    void receiveAid(float aid);
    void receiveEnergy(float energy);

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

    void setWeapon(Weapon *weaponType);

    void updateWeaponType();

    void createWeapon(const std::string &label);

    void addWeapon(Weapon *weaponType);

    Weapon *getWeaponTypeByLabel(const std::string& label);

    void setWeaponTypeByIndex(int i);

    void setAutoRotationToFacingSelectedObjectSpeed(float autoRotationSelectedObjectSpeed);

    void startBlinkShaderForPlayer();

    void stopBlinkForPlayer();

    int getKillsCounter() const;

    void setKillsCounter(int killsCounter);

    void increaseKills();

    void increaseLevelsCompleted();

    int getLevelCompletedCounter() const;

    void setLevelCompletedCounter(int levelCounter);

    [[nodiscard]] const std::vector<Weapon *> &getWeapons() const;

    bool isEnergyShieldEnabled() const;

    void setEnergyShieldEnabled(bool shieldEnabled);

    float getEnergy() const;

    void useEnergy(float energy);

    void setEnergy(float energy);

    int getStartStamina() const;

    void setStartStamina(int startStamina);

    float getStartEnergy() const;

    void setStartEnergy(float startEnergy);

    float getRecoverEnergySpeed() const;

    void setRecoverEnergySpeed(float recoverEnergySpeed);

    void startPlayerBlink();

    int getGravityShieldsNumber() const;

    void setGravityShieldsNumber(int gravityShieldsNumber);

    void loadShieldModel();

    void nextWeapon();

    void previousWeapon();

    void setAllowGravitationalShields(bool allowGravitationalShields);

    void setAllowEnergyShield(bool allowEnergyShield);

    bool isAllowGravitationalShields() const;

    bool isAllowEnergyShield() const;

    void decreaseLevelsCompleted();

    void loadBlinkShader();

    void postUpdate() override;
};


#endif //BRAKEZA3D_PLAYER_H
