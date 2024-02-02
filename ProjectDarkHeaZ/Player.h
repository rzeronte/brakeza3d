#ifndef BRAKEZA3D_PLAYER_H
#define BRAKEZA3D_PLAYER_H

#include "../include/Objects/Object3D.h"
#include "../include/Misc/Counter.h"
#include "../include/Objects/ParticleEmitter.h"
#include "../include/Physics/Mesh3DGhost.h"
#include "Weapons/Weapon.h"
#include "Items/PlayerReflection.h"
#include "FX/RayLight.h"
#include "Items/LivingObject.h"
#include "Items/PlayerSatellite.h"
#include "../include/Behaviors/RotatableToTarget.h"
#include "../include/2D/Sprite2D.h"

#define INITIAL_RECOVER_ENERGY 0.05f
#define INITIAL_POWER_DASH 1
#define DASH_ENERGY_COST 15
#define ENERGY_DELTATIME_COST 10.0f
#define INITIAL_FRICTION 5

typedef enum {
    EMPTY = -1,
    LIVE = 0,
    DEAD = 1,
    GETTING_DAMAGE = 2,
} PlayerState;

enum WeaponStatus {
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
    int rescuedHumans;
    int coins;

    Vertex3D velocity;
    Weapon *weapon;

    Counter counterStucked;
    Counter counterDashCadence;
    Counter counterLight;

    bool energyShieldEnabled;
    bool allowEnergyShield;

    LightPoint3D *light;

    Vertex3D lightPositionOffset;
    Vertex3D particlesEngineLeftOffset;

    PlayerState state;

    std::vector<Weapon *> weapons;

    PlayerSatellite satellite;

    ParticleEmitter *particleEngineLeft;

    Image *avatar;
    std::vector<Image *> damages;

    Sprite2D *spriteEnergyShield;

    float projectileStartOffsetPosition;

    bool autoTargetOnNextFrame = false;
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

    void setState(PlayerState state);

    [[nodiscard]] Weapon *getWeapon() const;

    void setWeapon(Weapon *weaponType);

    void addWeapon(Weapon *weaponType);

    Weapon *getWeaponTypeByLabel(const std::string& label);

    void setWeaponTypeByIndex(int i);

    void stopBlinkForPlayer();

    [[nodiscard]] const std::vector<Weapon *> &getWeapons() const;

    [[nodiscard]] bool isEnergyShieldEnabled() const;

    void setEnergyShieldEnabled(bool shieldEnabled);

    [[nodiscard]] float getEnergy() const;

    void useEnergy(float energy);

    void setEnergy(float energy);

    [[nodiscard]] float getStartEnergy() const;

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

    [[nodiscard]] LightPoint3D *getWeaponLight() const;

    void updateShaderParticles();

    void onDrawHostBuffer() override;

    Image *getAvatar();

    [[nodiscard]] int getCoins() const;

    void increaseCoins(int value);

    void decreaseCoins(int value);

    void loadSatellite();

    void dashMovement();

    void increaseHumans();

    int getRescuedHumans() const;

    static void makeRandomPlayerDamageSound();

    void updatePlayerEnergy();

    void updateTargetRotation();

    void initLight();

    void updateSpriteEnergyShield();

    void drawImGuiProperties() override;

    static void shiftCamera();

    Image *randomDamage();

    void setMaxVelocity(float maxVelocity);

    void setStartEnergy(float startEnergy);

    void setPower(float power);

    bool god;

    void setAutoTargetOnNextFrame(bool autoTargetOnNextFrame);

    bool isAutoTargetOnNextFrame() const;
};


#endif //BRAKEZA3D_PLAYER_H
