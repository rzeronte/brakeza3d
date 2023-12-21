
#include "Player.h"
#include "../include/Brakeza3D.h"
#include "Weapons/AmmoProjectileBody.h"
#include "Items/ItemWeaponGhost.h"
#include "Items/ItemHealthGhost.h"
#include "Items/ItemEnergyGhost.h"
#include "Items/PlayerReflection.h"
#include "Items/LivingObject.h"

Player::Player()
:
    LivingObject(this),
    RotatableToTarget(nullptr, this, 1.5f),
    energy(INITIAL_ENERGY),
    startEnergy(INITIAL_ENERGY),
    recoverEnergySpeed(INITIAL_RECOVER_ENERGY),
    stuck(false),
    warningDamage(false),
    rescuedHumans(0),
    coins(5000),
    weapon(nullptr),
    counterStucked(Counter(5)),
    counterDashCadence(Counter(1)),
    rayLight(RayLight(
        false,
        this,
        this->AxisDown().getNormalize(),
        this->AxisDown().getScaled(1100),
        500,
        0,
        Color::green(),
        Color::orange(),
        EngineSetup::collisionGroups::Projectile,
        EngineSetup::collisionGroups::Enemy
    )),
    killsCounter(0),
    energyShieldEnabled(false),
    allowEnergyShield(false),
    counterLight(Counter(0.05)),
    lightPositionOffset(Vertex3D(0, 0, -1000)),
    state(PlayerState::EMPTY),
    currentWeaponIndex(0),
    satellite(PlayerSatellite(this)),
    avatar(new Image(EngineSetup::get()->ICONS_FOLDER + "avatars/default.png")),
    dashPower(INITIAL_POWER_DASH),
    power(INITIAL_POWER),
    friction(INITIAL_FRICTION),
    maxVelocity(INITIAL_MAX_VELOCITY)
{
    velocity = Vertex3D(0, 0, 0);

    light = new LightPoint3D(glm::vec4(1), glm::vec4(1), glm::vec4(1), 1, 1, 1);
    light->setRotation(180, 0, 0);
    light->setEnabled(false);
    Brakeza3D::get()->addObject3D(light, "playerLight");

    shaderParticles = new ShaderParticles(true, PaletteColors::getParticlesPlayerFrom(), PaletteColors::getParticlesPlayerTo(), OCParticlesContext::forPlayerEngine());
    shaderParticlesTwo = new ShaderParticles(true, PaletteColors::getParticlesPlayerFrom(), PaletteColors::getParticlesPlayerTo(), OCParticlesContext::forPlayerEngine());

    shaderEnergyShield = new ShaderEnergyShield(
        true,
        this,
        std::string(EngineSetup::get()->IMAGES_FOLDER + "noise_color.png"),
        std::string(EngineSetup::get()->IMAGES_FOLDER + "shield_mask.png")
    );

    //spriteEnergyShield = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "Smoke45Frames.png"), 128, 128, 45, 45));
    spriteEnergyShield = new Sprite2D(0, 0, false, new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "shield.png"), 80, 80, 20, 60));

    spriteEnergyShield->setEnabled(false);
    Brakeza3D::get()->addObject3D(spriteEnergyShield, "playerSpriteEnergyShield");
}

void Player::loadSatellite()
{
    satellite.setEnabled(true);
    satellite.setLabel("satellitePlayer");
    satellite.AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "droid_ball.fbx"));
    satellite.setParent(parent);
    satellite.setFlatTextureColor(false);
    satellite.setPosition(getPosition());
    satellite.setStencilBufferEnabled(true);
    satellite.setRotationFrameEnabled(true);
    satellite.setRotationFrame(Vertex3D(0, 0, 2.5f));
    satellite.onStartSetup();
}

bool Player::takeDamage(float dmg)
{
    auto componentGame = ComponentsManager::get()->getComponentGame();

    if (componentGame->getGameState() != EngineSetup::GAMING) return false;

    if ((state == PlayerState::GETTING_DAMAGE && !isStucked()) || state == PlayerState::DEAD) {
        return false;
    }

    this->stamina -= dmg;

    if (stamina <= 0) {
        stamina = 0;
        setState(PlayerState::DEAD);
        if (warningDamage) {
            warningDamage = false;
            ComponentsManager::get()->getComponentSound()->stopChannel(warningSoundChannel);
        }
        ComponentsManager::get()->getComponentSound()->sound("playerDead", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        componentGame->makeFadeToGameState(EngineSetup::GameState::PRESS_KEY_BY_DEAD, true);
        componentGame->getFadeToGameState()->setupForFadeIn();
        componentGame->getShaderColor()->setEnabled(true);
    }

    setState(PlayerState::GETTING_DAMAGE);
    startPlayerBlink();

    return true;
}

void Player::startPlayerBlink()
{
    counterDamageBlink.setEnabled(true);
    blink->setEnabled(true);
}

void Player::respawn()
{
    Logging::Log("Respawn Player");
    setState(PlayerState::LIVE);
    setStamina(INITIAL_STAMINA);
}

void Player::shoot(float intensity)
{
    if (getWeapon() == nullptr || !getWeapon()->isAvailable()) {
        return;
    }

    switch (getWeapon()->getType()) {
        case WeaponTypes::WEAPON_PROJECTILE: {
            bool resultShoot = weapon->shootProjectile(
                this,
                getPosition(),
                AxisUp().getScaled(0.01),
                AxisUp().getInverse(),
                getRotation(),
                PaletteColors::getPlayerProjectile(),
                intensity,
                EngineSetup::collisionGroups::Projectile,
                EngineSetup::collisionGroups::Enemy,
                true,
                true,
                PaletteColors::getParticlesPlayerProjectileFrom(),
                PaletteColors::getParticlesPlayerProjectileTo()
            );

            if (resultShoot) {
                getWeaponLight()->setColor(PaletteColors::getPlayerProjectileLight());
                initLight();
            }
            break;
        }
        case WeaponTypes::WEAPON_LASER: {
            bool resultShoot = weapon->shootLaserProjectile(
                this,
                getPosition() - AxisUp().getScaled(0.01),
                AxisUp().getInverse(),
                0.001f,
                true,
                PaletteColors::getPlayerLaser(),
                EngineSetup::collisionGroups::Projectile,
                EngineSetup::collisionGroups::Enemy
            );

            if (resultShoot) {
                getWeaponLight()->setColor(PaletteColors::getPlayerLaserLight());
                initLight();
            }

            break;
        }
        case WeaponTypes::WEAPON_BOMB: {
            weapon->shootBomb(this, getPosition());
            break;
        }
        case WeaponTypes::WEAPON_SHIELD: {
            weapon->shootShield(this, getPosition());
            break;
        }
        case WeaponTypes::WEAPON_REFLECTION: {
            weapon->shootHologram(this, getPosition());
            break;
        }
        case WeaponTypes::WEAPON_RAYLIGHT: {
            rayLight.updateDirection(
                rayLight.getParent()->AxisDown().getNormalize(),
                rayLight.getParent()->AxisDown().getScaled(0.01)
            );

            weapon->shootRayLight(rayLight, 0.001, PaletteColors::getPlayerRay());
            getWeaponLight()->setColor(PaletteColors::getPlayerRayLight());
            initLight();
            break;
        }
        default:
            Logging::Log("Not weapon type shoot!");
    }
}

void Player::receiveAid(float value) {
    this->stamina = stamina + value;

    if (stamina > (float) getStartStamina()) {
        this->stamina = (float) getStartStamina();
    }
}

void Player::receiveEnergy(float value)
{
    this->energy = energy + value;

    if (energy > (float) getStartEnergy()) {
        this->energy = (float) getStartEnergy();
    }
}

void Player::onUpdate()
{
    Mesh3D::onUpdate();

    updateWeaponInteractionStatus();

    counterLight.update();
    if (counterLight.isFinished()) {
        light->setEnabled(false);
        counterLight.setEnabled(false);
    }

    counterDashCadence.update();
    if (counterDashCadence.isFinished()) {
        counterDashCadence.setEnabled(false);
    }

    applyFriction();

    updatePlayerEnergy();

    updateLight();

    updateTargetRotation();

    updateShaderParticles();

    setPosition(getPosition() + this->velocity);

    updateSpriteEnergyShield();

    auto componentGame = ComponentsManager::get()->getComponentGame();
    if (componentGame->getStoreManager()->isItemEnabled(EngineSetup::StoreItems::ITEM_SATELLITE)) {
        satellite.onUpdate();
    }

    auto isGameStateDistinctOfGameOver = componentGame->getGameState() != EngineSetup::PRESS_KEY_BY_DEAD;
    if (warningDamage && isGameStateDistinctOfGameOver && state != PlayerState::DEAD) {
        float currentSelectionAlpha = 0.5f * (float) (1 + sin(5 * M_PI * Brakeza3D::get()->getExecutionTime()));
        ComponentsManager::get()->getComponentGame()->getShaderColor()->setProgress(currentSelectionAlpha);
    }

    if ((currentStaminaPercentage() < 25 && !warningDamage) && isGameStateDistinctOfGameOver && state != PlayerState::DEAD) {
        ComponentsManager::get()->getComponentGame()->getShaderColor()->setEnabled(true);
        warningDamage = true;
        warningSoundChannel = ComponentsManager::get()->getComponentSound()->sound("alarmDamage", EngineSetup::SoundChannels::SND_GLOBAL, -1);
    }

    if ((currentStaminaPercentage() >= 25 && warningDamage) && isGameStateDistinctOfGameOver && state != PlayerState::DEAD) {
        ComponentsManager::get()->getComponentGame()->getShaderColor()->setEnabled(false);
        warningDamage = false;
        ComponentsManager::get()->getComponentSound()->stopChannel(warningSoundChannel);
    }
}

void Player::updateTargetRotation()
{
    setRotationTarget(ComponentsManager::get()->getComponentRender()->getSelectedObject());

    makeRotationToTarget();


    //const float theta = newRot.X() * b;
}

void Player::updatePlayerEnergy()
{
    if (!isEnergyShieldEnabled()) {
        if (getEnergy() < getStartEnergy()) {
            float recoverEnergy = getEnergy() + recoverEnergySpeed;
            if (ComponentsManager::get()->getComponentGame()->getStoreManager()->isItemEnabled(EngineSetup::ITEM_FAST_ENERGY_RELOAD)) {
                recoverEnergy *= 1.0025f;
            }
            setEnergy(std::min(recoverEnergy, getStartEnergy()));
        }
    } else {
        const float energyCost = Brakeza3D::get()->getDeltaTime() * ENERGY_DELTATIME_COST;
        if (getEnergy() > energyCost) {
            useEnergy(energyCost);
        } else {
            setEnergyShieldEnabled(false);
            //ComponentsManager::get()->getComponentSound()->sound("energyOff", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }
    }
}

void Player::drawOnUpdateSecondPass()
{
    if (counterDamageBlink.isEnabled()) {
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            stopBlinkForPlayer();
        }
    }

    if (ComponentsManager::get()->getComponentGame()->getStoreManager()->isItemEnabled(EngineSetup::StoreItems::ITEM_SATELLITE)) {
        satellite.drawOnUpdateSecondPass();
    }

    if (isEnergyShieldEnabled()) {
        shaderEnergyShield->update();
    }
}

void Player::onDrawHostBuffer()
{
    Mesh3D::onDrawHostBuffer();
    if (isStucked()) {
        counterStucked.update();

        if (counterStucked.isFinished()) {
            unStuck();
            startPlayerBlink();
            Logging::Log("unStuck");
        }
    }
}

void Player::updateShaderParticles()
{
    shaderParticles->update(
        getPosition() - AxisUp().getScaled(-1) - AxisLeft().getScaled(2),
        AxisUp(),
        0.001f
    );

    /*shaderParticlesTwo->update(
        getPosition() - AxisUp().getScaled(-1) + AxisLeft().getScaled(2),
        AxisUp(),
        (velocity.getModule() / maxVelocity) + 0.1f
    );*/
}

void Player::postUpdate()
{
    if (!isEnabled()) return;

    Mesh3D::postUpdate();

    if (state == PlayerState::DEAD) {
        return;
    }

    if (weapon != nullptr) {
        weapon->onUpdate();
    }

    if (ComponentsManager::get()->getComponentGame()->getStoreManager()->isItemEnabled(EngineSetup::StoreItems::ITEM_SATELLITE)) {
        satellite.postUpdate();
    }
}

Vertex3D Player::getVelocity()
{
    return this->velocity;
}

void Player::applyFriction() {
    if (Tools::isZeroVector(this->velocity)) {
        return;
    }

    velocity = velocity + velocity.getInverse().getScaled(Brakeza3D::get()->getDeltaTime() * friction);
}

void Player::setVelocity(Vertex3D v)
{
    this->velocity = v;
}

void Player::updateLight()
{
    light->setPosition(getPosition() + lightPositionOffset + AxisUp().getScaled(-1000));
    light->onUpdate();
}

void Player::resolveCollision(Collisionable *with)
{
    Mesh3DGhost::resolveCollision(with);
    auto projectile = dynamic_cast<AmmoProjectileBody*> (with);
    if (projectile != nullptr && projectile->getParent() != this) {
        const float dmg = projectile->getWeaponType()->getDamage();
        if (isEnergyShieldEnabled() ) {
            if (getEnergy() > dmg) {
                useEnergy(dmg);
                ComponentsManager::get()->getComponentSound()->sound("energyDamage", EngineSetup::SoundChannels::SND_GLOBAL, 0);
                setState(PlayerState::GETTING_DAMAGE);
                startPlayerBlink();
            } else {
                setEnergy(0);
                setEnergyShieldEnabled(false);
            }
        } else {
            makeRandomPlayerDamageSound();
            takeDamage(dmg);
            projectile->setRemoved(true);
        }
    }

    auto laser = dynamic_cast<ProjectileRay*> (with);
    if (laser != nullptr) {
        const float dmg = laser->getDamage() * Brakeza3D::get()->getDeltaTime();
        if (isEnergyShieldEnabled() ) {
            if (getEnergy() > dmg) {
                useEnergy(dmg);
                setState(PlayerState::GETTING_DAMAGE);
                startPlayerBlink();
            } else {
                setEnergy(0);
                setEnergyShieldEnabled(false);
            }
        } else {
            takeDamage(dmg);
        }
    }

    auto weapon = dynamic_cast<ItemWeaponGhost*> (with);
    if (weapon != nullptr) {
        this->addWeapon(weapon->getWeaponType());

        ComponentsManager::get()->getComponentSound()->sound("getAmmo", EngineSetup::SND_GLOBAL, 0);

        Logging::Log("Added Weapon to Player: %s", weapon->getWeaponType()->getLabel().c_str());

        weapon->setRemoved(true);
        Tools::makeExplosion(this, weapon->getPosition(), 5, OCParticlesContext::forExplosion(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getExplosionEnemyTo());

        if (weapon->isHasTutorial()) {
            ComponentsManager::get()->getComponentGame()->setGameState(EngineSetup::GAMING_TUTORIAL);
            if (weapon->getTutorialIndex() != -1) {
                ComponentsManager::get()->getComponentGame()->setHelp(
                        ComponentsManager::get()->getComponentGame()->getLevelLoader()->levelTutorials[weapon->getTutorialIndex()]
                );
            }
        }
    }

    auto health = dynamic_cast<ItemHealthGhost*> (with);
    if (health != nullptr) {
        Logging::Log("Health to Player!");
        ComponentsManager::get()->getComponentSound()->sound("itemHealth", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        receiveAid(health->getAid());
        health->remove();
        Tools::makeExplosion(this, health->getPosition(), 5, OCParticlesContext::forExplosion(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getExplosionEnemyTo());

        if (health->isHasTutorial()) {
            ComponentsManager::get()->getComponentGame()->setGameState(EngineSetup::GAMING_TUTORIAL);
            if (health->getTutorialIndex() != -1) {
                ComponentsManager::get()->getComponentGame()->setHelp(
                        ComponentsManager::get()->getComponentGame()->getLevelLoader()->levelTutorials[health->getTutorialIndex()]
                );
            }
        }
    }

    auto human = dynamic_cast<ItemHumanGhost*> (with);
    if (human != nullptr) {
        Logging::Log("human rescued!");
        ComponentsManager::get()->getComponentSound()->sound("yes", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        increaseHumans();
        human->removeCollisionObject();
        human->remove();

        if (human->isHasTutorial()) {
            ComponentsManager::get()->getComponentGame()->setGameState(EngineSetup::GAMING_TUTORIAL);
            if (human->getTutorialIndex() != -1) {
                ComponentsManager::get()->getComponentGame()->setHelp(
                        ComponentsManager::get()->getComponentGame()->getLevelLoader()->levelTutorials[human->getTutorialIndex()]
                );
            }
        }
    }

    auto energy = dynamic_cast<ItemEnergyGhost*> (with);
    if (energy != nullptr) {
        Logging::Log("Health to Player!");
        ComponentsManager::get()->getComponentSound()->sound("itemHealth", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        receiveEnergy(energy->getEnergy());
        energy->remove();
        Tools::makeExplosion(this, energy->getPosition(), 5, OCParticlesContext::forExplosion(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getExplosionEnemyTo());

        if (energy->isHasTutorial()) {
            ComponentsManager::get()->getComponentGame()->setGameState(EngineSetup::GAMING_TUTORIAL);
            if (energy->getTutorialIndex() != -1) {
                ComponentsManager::get()->getComponentGame()->setHelp(
                        ComponentsManager::get()->getComponentGame()->getLevelLoader()->levelTutorials[energy->getTutorialIndex()]
                );
            }
        }
    }

    auto enemy = dynamic_cast<EnemyGhost*> (with);
    if (enemy != nullptr) {
        takeDamage(getStartStamina() / 10 );
    }
}

void Player::setState(PlayerState state)
{
    Logging::Log("Player change state to %d", state);
    Player::state = state;
}

Weapon *Player::getWeapon() const {
    return weapon;
}

void Player::setWeapon(Weapon *weaponType)
{
    Logging::Log("Set Player Weapon to %s", weaponType->getLabel().c_str());
    Player::weapon = weaponType;
    ComponentsManager::get()->getComponentSound()->sound("switchWeapon", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

void Player::addWeapon(Weapon *weaponType)
{
    auto weapon = getWeaponTypeByLabel(weaponType->getLabel());
    if (weapon != nullptr) {
        weapon->addAmount(weaponType->getStartAmmoAmount());
        Logging::Log("Weapon already exist! Added ammo: %d", weaponType->getStartAmmoAmount());
        return;
    }

    this->weaponTypes.emplace_back(weaponType);
}

Weapon *Player::getWeaponTypeByLabel(const std::string& label)
{
    for (auto & weaponType : this->weaponTypes) {
        if (weaponType->getLabel() == label) {
            return weaponType;
        }
    }

    return nullptr;
}

void Player::setWeaponTypeByIndex(int i) {
    setWeapon(weaponTypes[i]);
    this->currentWeaponIndex = i;
}

void Player::stopBlinkForPlayer()
{
    setState(PlayerState::LIVE);
    counterDamageBlink.setEnabled(false);
    blink->setEnabled(false);
}

int Player::getKillsCounter() const {
    return killsCounter;
}

void Player::setKillsCounter(int value)
{
    Player::killsCounter = value;
}

void Player::increaseKills()
{
    killsCounter++;
}

const std::vector<Weapon *> &Player::getWeapons() const {
    return weaponTypes;
}

bool Player::isEnergyShieldEnabled() const {
    return energyShieldEnabled;
}

void Player::setEnergyShieldEnabled(bool shieldEnabled)
{
    Player::energyShieldEnabled = shieldEnabled;

    spriteEnergyShield->setEnabled(shieldEnabled);

    if (shieldEnabled) {
        blink->setColor(PaletteColors::getPlayerBenefitBlink());
    } else {
        blink->setColor(PaletteColors::getPlayerDamageBlink());
    }
}

float Player::getEnergy() const {
    return energy;
}

void Player::useEnergy(float value)
{
    this->energy -= value;
}

void Player::setEnergy(float value) {
    Player::energy = value;
}

float Player::getStartEnergy() const {
    return startEnergy;
}

void Player::setStartEnergy(float value) {
    Player::startEnergy = value;
}

float Player::getRecoverEnergySpeed() const {
    return recoverEnergySpeed;
}

void Player::setRecoverEnergySpeed(float value) {
    Player::recoverEnergySpeed = value;
}

void Player::nextWeapon()
{
    auto currentWeapon = getWeapon();
    auto currentIterator = std::find(weaponTypes.begin(), weaponTypes.end(), currentWeapon);

    for (auto it = currentIterator; it != weaponTypes.end(); it++) {
        if (*(it) == currentWeapon) {
            continue;
        }
        auto weapon = *(it);
        if (weapon->isAvailable() && weapon->isSelectable()) {
            setWeapon(weapon);
            return;
        }
    }
}

void Player::previousWeapon()
{
    auto currentWeapon = getWeapon();
    auto currentIterator = std::find(weaponTypes.begin(), weaponTypes.end(), currentWeapon);

    int index = std::distance(weaponTypes.begin(), currentIterator);

    for (int i = index; i >= 0; i--) {
        if (weaponTypes[i] == currentWeapon) {
            continue;
        }
        auto weapon = weaponTypes[i];
        if (weapon->isAvailable()) {
            setWeapon(weapon);
            return;
        }
    }
}

void Player::setAllowEnergyShield(bool value) {
    Player::allowEnergyShield = value;
}

bool Player::isAllowEnergyShield() const
{
    return allowEnergyShield;
}

void Player::onStartSetup()
{
    blink = new ShaderBlink(false, this, 0.05, PaletteColors::getPlayerDamageBlink());
    counterDamageBlink.setEnabled(false);

    loadSatellite();
}

PlayerState Player::getState() const
{
    return state;
}

void Player::makeStuck(float time)
{
    counterStucked.setStep(time);
    counterStucked.setEnabled(true);

    Tools::makeFadeInSprite(getPosition(), ComponentsManager::get()->getComponentGame()->getSpriteStuck()->getAnimation());

    setStucked(true);

    velocity = Vertex3D::zero();

    setState(PlayerState::GETTING_DAMAGE);

    ComponentsManager::get()->getComponentSound()->sound("electricStuck", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

bool Player::isStucked() const
{
    return stuck;
}

void Player::setStucked(bool value)
{
    Player::stuck = value;
}

void Player::unStuck()
{
    setStucked(false);
    counterStucked.setEnabled(false);
}

void Player::setEnabled(bool value)
{
    Object3D::setEnabled(value);
}

void Player::updateWeaponInteractionStatus()
{
    auto componentInput = ComponentsManager::get()->getComponentInput();
    auto componentGameInput = ComponentsManager::get()->getComponentGameInput();
    auto weaponStatus = getWeapon()->getStatus();

    if (weaponStatus == WeaponStatus::SUSTAINED && componentInput->getControllerAxisTriggerRight() < componentGameInput->getControllerAxisThreshold()) {
        getWeapon()->setStatus(WeaponStatus::RELEASED);
    }

    if (weaponStatus == WeaponStatus::NONE && componentInput->getControllerAxisTriggerRight() >= componentGameInput->getControllerAxisThreshold()) {
        getWeapon()->setStatus(WeaponStatus::PRESSED);
    }
}

void Player::updateWeaponAutomaticStatus()
{
    if (getWeapon()->getStatus() == PRESSED) {
        getWeapon()->setStatus(SUSTAINED);
    }

    if (getWeapon()->getStatus() == RELEASED) {
        rayLight.resetReach();
        getWeapon()->setStatus(NONE);
    }
}

LightPoint3D *Player::getWeaponLight() const {
    return light;
}

Player::~Player()
{
    delete light;
    delete blink;
    delete avatar;
    delete shaderEnergyShield;
    delete shaderParticles;
    delete shaderParticlesTwo;

    for (auto w : weaponTypes) {
        delete w;
    }
}

ShaderParticles *Player::getShaderParticles() const {
    return shaderParticles;
}

Image *Player::getAvatar() {
    return avatar;
}

int Player::getCoins() const {
    return coins;
}

void Player::increaseCoins(int value)
{
    ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->coinsGained += value;
    coins += value;
}

void Player::decreaseCoins(int value)
{
    coins -= value;
}

void Player::dashMovement()
{
    if (isStucked()) return;

    if (!counterDashCadence.isEnabled()) {

        if (getVelocity().getModule() <= 0.007f) return;

        counterDashCadence.setEnabled(true);

        float dashEnergyCost = DASH_ENERGY_COST;

        if (getEnergy() < dashEnergyCost) {
            return;
        }

        ComponentsManager::get()->getComponentSound()->sound("dash", EngineSetup::SoundChannels::SND_GLOBAL, 0);

        Tools::makeExplosion(this, getPosition(), 1, OCParticlesContext::forExplosion(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getExplosionEnemyTo());

        Brakeza3D::get()->addObject3D(new ShockWave(getPosition(), 0.50, 50, 1, true), Brakeza3D::uniqueObjectLabel("shockWave"));

        float power = dashPower;
        if (ComponentsManager::get()->getComponentGame()->getStoreManager()->isItemEnabled(EngineSetup::StoreItems::ITEM_EXTRA_DASH)) {
            power *= 2;
        }

        setPosition(getPosition() + getVelocity().getNormalize().getScaled(power));

        useEnergy(dashEnergyCost);
    }
}

void Player::increaseHumans()
{
    rescuedHumans++;
}

int Player::getRescuedHumans() {
    return rescuedHumans;
}

void Player::makeRandomPlayerDamageSound()
{
    std::string randomMetalSound = "metalHit0" + std::to_string(Tools::random(1, 5));

    ComponentsManager::get()->getComponentSound()->sound(randomMetalSound, EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

RayLight &Player::getRayLight() {
    return rayLight;
}

void Player::initLight()
{
    counterLight.setEnabled(true);
    light->setEnabled(true);
}

void Player::updateSpriteEnergyShield()
{
    if (!spriteEnergyShield->isEnabled()) return;

    Vertex3D A;
    const auto cam = ComponentsManager::get()->getComponentCamera()->getCamera();

    Transforms::cameraSpace(A, getPosition(), cam);
    A = Transforms::PerspectiveNDCSpace(A, cam->getFrustum());

    Point2D P1;
    Transforms::screenSpace(P1, A);

    spriteEnergyShield->updatePosition(P1.x, P1.y);
}
