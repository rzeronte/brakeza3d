
#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../include/Objects/Object3D.h"
#include "../include/Misc/Counter.h"
#include "../include/Particles/ParticleEmissor.h"
#include "../include/Physics/Mesh3DGhost.h"
#include "src/weapons/Weapon.h"
#include "src/shaders/ShaderShockWave.h"
#include "src/items/PlayerReflection.h"
#include "src/shaders/ShaderLightRay.h"

#define INITIAL_STAMINA 100
#define INITIAL_ENERGY 100
#define INITIAL_RECOVER_ENERGY 0.05f
#define INITIAL_LIVES 10
#define INITIAL_POWER 500
#define INITIAL_POWERDASH 6000
#define INITIAL_FRICTION 5
#define INITIAL_MAX_VELOCITY 150
#define MAX_REFLECTIONS 3
#define PLAYER_ROTATION_TARGET_SPEED 1

typedef enum {
    EMPTY = -1,
    LIVE = 0,
    DEAD = 1,
    GETTING_DAMAGE = 2,
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
    bool stuck;

    int lives;
    Vertex3D velocity;
    Weapon *weapon;
    Counter counterDamageBlink;
    Counter counterStucked;

    ShaderBlink *blink;
    ShaderLightRay *shaderLaser;

    int killsCounter;
    bool energyShieldEnabled;
    int gravityShieldsNumber;

    bool allowMakeReflections;
    bool allowEnergyShield;


    LightPoint3D *light;
    Vertex3D lightPositionOffset;

    PlayerState state;

    std::vector<Weapon *> weaponTypes;
    int currentWeaponIndex;

    Mesh3D shieldModel;
    PlayerReflection reflection;
public:

    Player();

    ~Player() override;

    void respawn();

    void shoot(float intensity);

    void makeReflection();

    bool takeDamage(float dmg);

    void receiveAid(float aid);
    void receiveEnergy(float energy);

    [[nodiscard]] int getStamina() const;

    void setStamina(int stamina);

    void setLives(int lives);

    void onUpdate() override;
    void postUpdate() override;

    Vertex3D getVelocity();
    void setVelocity(Vertex3D v);

    float dashPower;
    float power;
    float friction;
    float maxVelocity;

    void applyFriction();

    void updateLight();

    void resolveCollision(Collisionable *with) override;

    float rotationToTargetSpeed;

    void setState(PlayerState state);

    [[nodiscard]] Weapon *getWeapon() const;

    void setWeapon(Weapon *weaponType);

    void addWeapon(Weapon *weaponType);

    Weapon *getWeaponTypeByLabel(const std::string& label);

    void setWeaponTypeByIndex(int i);

    void setAutoRotationToFacingSelectedObjectSpeed(float autoRotationSelectedObjectSpeed);

    void startBlinkShaderForPlayer();

    void stopBlinkForPlayer();

    [[nodiscard]] int getKillsCounter() const;

    void setKillsCounter(int killsCounter);

    void increaseKills();

    [[nodiscard]] const std::vector<Weapon *> &getWeapons() const;

    [[nodiscard]] bool isEnergyShieldEnabled() const;

    void setEnergyShieldEnabled(bool shieldEnabled);

    [[nodiscard]] float getEnergy() const;

    void useEnergy(float energy);

    void setEnergy(float energy);

    [[nodiscard]] int getStartStamina() const;

    void setStartStamina(int startStamina);

    [[nodiscard]] float getStartEnergy() const;

    void setStartEnergy(float startEnergy);

    [[nodiscard]] float getRecoverEnergySpeed() const;

    void setRecoverEnergySpeed(float recoverEnergySpeed);

    void startPlayerBlink();

    [[nodiscard]] int getGravityShieldsNumber() const;

    void setGravityShieldsNumber(int gravityShieldsNumber);

    void loadShieldModel();

    void nextWeapon();

    void previousWeapon();

    void setAllowGravitationalShields(bool allowGravitationalShields);

    void setAllowEnergyShield(bool allowEnergyShield);

    [[nodiscard]] bool isAllowedMakeReflections() const;

    [[nodiscard]] bool isAllowEnergyShield() const;

    void loadBlinkShader();

    void loadReflection();

    [[nodiscard]] PlayerState getState() const;

    void makeStuck(float time);

    [[nodiscard]] bool isStucked() const;

    void setStucked(bool value);

    void unStuck();

    void setEnabled(bool value) override;

    void updateWeaponInteractionStatus() const;

    void updateWeaponAutomaticStatus();

    [[nodiscard]] ShaderLightRay *getShaderLaser();

    [[nodiscard]] Mesh3D *getShieldModel();

    [[nodiscard]] PlayerReflection *getReflection();

    [[nodiscard]] LightPoint3D *getLight() const;
};


#endif //BRAKEZA3D_PLAYER_H
