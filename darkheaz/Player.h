#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../include/Objects/Object3D.h"
#include "../include/Misc/Counter.h"
#include "../include/Particles/ParticleEmitter.h"
#include "../include/Physics/Mesh3DGhost.h"
#include "src/weapons/Weapon.h"
#include "src/shaders/ShaderShockWave.h"
#include "src/items/PlayerReflection.h"
#include "src/shaders/RayLight.h"
#include "src/shaders/ShaderParticles.h"
#include "src/shaders/ShaderZombie.h"
#include "src/shaders/ShaderEnergyShield.h"
#include "src/items/LivingObject.h"
#include "src/items/PlayerSatellite.h"
#include "src/behaviors/RotatableToTarget.h"

#define INITIAL_STAMINA 100
#define INITIAL_ENERGY 100
#define INITIAL_RECOVER_ENERGY 0.05f
#define INITIAL_POWER 250
#define INITIAL_POWER_DASH 1000
#define DASH_ENERGY_COST 15
#define ENERGY_DELTATIME_COST 5.0f
#define INITIAL_FRICTION 5
#define INITIAL_MAX_VELOCITY 50
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

class Player : public Mesh3DGhost, public LivingObject, public RotatableToTarget {
private:

    float energy;
    float startEnergy;
    float recoverEnergySpeed;
    bool stuck;
    bool warningDamage;
    int warningSoundChannel;
    int rescuedHumans;
    int coins;

    Vertex3D velocity;
    Weapon *weapon;
    Counter counterStucked;
    Counter counterDashCadence;

    RayLight rayLight;

    int killsCounter;
    bool energyShieldEnabled;

    bool allowEnergyShield;

    LightPoint3D *light;
    Vertex3D lightPositionOffset;

    PlayerState state;

    std::vector<Weapon *> weaponTypes;
    int currentWeaponIndex;

    PlayerSatellite satellite;

    ShaderParticles *shaderParticles;

    Image *avatar;
    Image *shield;
    ShaderEnergyShield *shaderEnergyShield;
public:

    Player();

    ~Player() override;

    void respawn();

    void shoot(float intensity);

    bool takeDamage(float dmg);

    void receiveAid(float aid);

    void receiveEnergy(float energy);

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

    [[nodiscard]] float getStartEnergy() const;

    void setStartEnergy(float startEnergy);

    [[nodiscard]] float getRecoverEnergySpeed() const;

    void setRecoverEnergySpeed(float recoverEnergySpeed);

    void startPlayerBlink();

    void nextWeapon();

    void previousWeapon();

    void setAllowEnergyShield(bool allowEnergyShield);

    [[nodiscard]] bool isAllowEnergyShield() const;

    void onStartSetup();

    [[nodiscard]] PlayerState getState() const;

    void makeStuck(float time);

    [[nodiscard]] bool isStucked() const;

    void setStucked(bool value);

    void unStuck();

    void setEnabled(bool value) override;

    void updateWeaponInteractionStatus() const;

    void updateWeaponAutomaticStatus();

    [[nodiscard]] RayLight &getShaderLaser();

    [[nodiscard]] LightPoint3D *getLight() const;

    void updateShaderParticles();

    [[nodiscard]] ShaderParticles *getShaderParticles() const;

    void drawCall() override;

    void onDraw() override;

    Image *getAvatar();

    [[nodiscard]] int getCoins() const;

    void increaseCoins(int value);

    void decreaseCoins(int value);

    void loadSatellite();

    void dashMovement();

    void increaseHumans();

    int getRescuedHumans();

    void makeRandomPlayerDamageSound();

    void updatePlayerEnergy();

    void updateTargetRotation();
};


#endif //BRAKEZA3D_PLAYER_H
