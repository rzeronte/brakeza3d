
#include "Player.h"
#include "../include/Brakeza3D.h"
#include "src/weapons/AmmoProjectileBody.h"
#include "src/items/ItemWeaponGhost.h"
#include "src/items/ItemHealthGhost.h"
#include "src/items/ItemEnergyGhost.h"
#include "src/items/PlayerReflection.h"

Player::Player() :
    stamina(INITIAL_STAMINA),
    startStamina(INITIAL_STAMINA),
    energy(INITIAL_ENERGY),
    startEnergy(INITIAL_ENERGY),
    recoverEnergySpeed(INITIAL_RECOVER_ENERGY),
    stuck(false),
    lives(INITIAL_LIVES),
    weapon(nullptr),
    counterDamageBlink (Counter(0.45)),
    counterStucked(Counter(5)),
    blink(nullptr),
    rayLight(RayLight(
        false,
        this,
        1000,
        0,
        Color::green(),
        EngineSetup::collisionGroups::Projectile,
        EngineSetup::collisionGroups::Enemy
    )),
    killsCounter(0),
    energyShieldEnabled(false),
    gravityShieldsNumber(0),
    allowMakeReflections(false),
    allowEnergyShield(false),
    lightPositionOffset(Vertex3D(0, -550, 0)),
    state(PlayerState::EMPTY),
    currentWeaponIndex(0),
    shieldModel(Mesh3D()),
    reflection(PlayerReflection((float) stamina, 5)),
    dashPower(INITIAL_POWERDASH),
    power(INITIAL_POWER),
    friction(INITIAL_FRICTION),
    maxVelocity(INITIAL_MAX_VELOCITY),
    rotationToTargetSpeed(PLAYER_ROTATION_TARGET_SPEED)
{
    light = new LightPoint3D(45, 5.7, 0, 0, 9, Color(100, 16, 22), Color(15, 33, 92));
    light->setRotation(180, 0, 0);
    Brakeza3D::get()->addObject3D(light, "playerLight");
}

void Player::loadShieldModel()
{
    shieldModel.setEnabled(false);
    shieldModel.setLabel("shieldModel");
    shieldModel.setPosition(getPosition());
    shieldModel.setAlpha(200);
    shieldModel.setAlphaEnabled(true);
    shieldModel.setEnableLights(true);
    shieldModel.setScale(1);
    shieldModel.setStencilBufferEnabled(true);
    shieldModel.setFlatTextureColor(false);
    shieldModel.setFlatColor(Color::green());
    //shieldModel->setRotationFrameEnabled(true);
    //shieldModel->setRotationFrame(Vertex3D(0, 0, 1));
    shieldModel.AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "shield.fbx"));

    Brakeza3D::get()->addObject3D(&shieldModel, "shieldPlayer");
}

void Player::loadReflection()
{
    reflection.setEnabled(false);
    reflection.setLabel("gravitationalShieldPlayer");
    reflection.AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceships/player.fbx"));
    reflection.setParent(parent);
    reflection.setFlatTextureColor(false);
    reflection.setPosition(getPosition());
    reflection.setStencilBufferEnabled(true);
    reflection.setRotationFrameEnabled(true);
    reflection.setRotationFrame(Tools::randomVertex().getScaled(0.5));

    Brakeza3D::get()->addObject3D(&reflection, "gravitationalShieldPlayer");
}

int Player::getStamina() const {
    return (int) stamina;
}

void Player::setStamina(int value) {
    Player::stamina = (float) value;
}

void Player::setLives(int value) {
    Player::lives = value;
}

bool Player::takeDamage(float dmg)
{
    if (ComponentsManager::get()->getComponentGame()->getGameState() != EngineSetup::GAMING) return false;

    if ((state == PlayerState::GETTING_DAMAGE && !isStucked()) || state == PlayerState::DEAD) {
        return false;
    }

    if (isEnergyShieldEnabled() && getEnergy() > 0) {
        useEnergy(dmg);
        setState(PlayerState::GETTING_DAMAGE);
        startPlayerBlink();
        return false;
    }

    this->stamina -= dmg;

    if (stamina <= 0) {
        stamina = 0;
        setState(PlayerState::DEAD);
        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("playerDead"),
            1,
            0
        );
        ComponentsManager::get()->getComponentGame()->makeFadeToGameState(EngineSetup::GameState::PRESS_KEY_BY_DEAD);
        ComponentsManager::get()->getComponentGame()->getFadeToGameState()->setupForFadeIn();
        ComponentsManager::get()->getComponentGame()->getShaderColor()->setEnabled(true);
        lives--;
    }

    setState(PlayerState::GETTING_DAMAGE);
    startPlayerBlink();

    return true;
}

void Player::startPlayerBlink()
{
    startBlinkShaderForPlayer();
}

void Player::respawn()
{
    Logging::Log("Respawn Player");
    setState(PlayerState::LIVE);
    setStamina(INITIAL_STAMINA);
}

void Player::makeReflection()
{
    if (gravityShieldsNumber >= (int) MAX_REFLECTIONS) {
        return;
    }

    reflection.setPosition(getPosition());
    reflection.reset();

    gravityShieldsNumber++;

    ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("gravitationalShield"),
        1,
        0
    );
}

void Player::shoot(float intensity)
{
    if (getWeapon() == nullptr) {
        return;
    }

    if (!getWeapon()->isAvailable()) {
        return;
    }

    switch (getWeapon()->getType()) {
        case WeaponTypes::WEAPON_PROJECTILE: {
            weapon->shootProjectile(
                this,
                getPosition() - AxisUp().getScaled(1000),
                AxisUp().getInverse(),
                getRotation(),
                intensity,
                EngineSetup::collisionGroups::Projectile,
                EngineSetup::collisionGroups::Enemy,
                true
            );
            break;
        }
        case WeaponTypes::WEAPON_LASER_PROJECTILE: {
            weapon->shootLaserProjectile(
                this,
                getPosition() - AxisUp().getScaled(1000),
                AxisUp().getInverse(),
                intensity,
                true,
                Color::green(),
                EngineSetup::collisionGroups::Projectile,
                EngineSetup::collisionGroups::Enemy
            );
            break;
        }
        case WeaponTypes::WEAPON_BOMB: {
            weapon->shootBomb(this, getPosition());
            break;
        }
        case WeaponTypes::WEAPON_LASER_RAY: {
            weapon->shootRayLight(rayLight, intensity * 0.5f);
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

    if (isStucked()) {
        Drawable::drawLightning(
            getPosition() + Tools::randomVertex().getScaled(5),
            getPosition() + Tools::randomVertex().getScaled(5),
            Color::green()
        );
        counterStucked.update();

        if (counterStucked.isFinished()) {
            unStuck();
            startPlayerBlink();
            Logging::Log("unStuck");
        }
    }

    //updateLight();

    applyFriction();

    if (getEnergy() < getStartEnergy()) {
        setEnergy(std::min(getEnergy() + recoverEnergySpeed, getStartEnergy()));
    }

    if (isEnergyShieldEnabled()) {
        shieldModel.setPosition(getPosition());
        shieldModel.setRotation(getRotation());
        shieldModel.setAlpha( (float) std::clamp((int)shieldModel.getAlpha() + 10, 0, 255));
    } else {
        if (shieldModel.getAlpha() == 0) {
            shieldModel.setEnabled(false);
        }
        shieldModel.setRotation(getRotation());
        shieldModel.setPosition(getPosition());
        shieldModel.setAlpha( (float) std::clamp((int)shieldModel.getAlpha() - 25, 0, 255));
    }

    auto selectedObject = ComponentsManager::get()->getComponentRender()->getSelectedObject();
    if (selectedObject != this && selectedObject != nullptr) {
        Vector3D way(selectedObject->getPosition(), getPosition());
        M3 newRot = M3::getFromVectors(EngineSetup::get()->forward,way.getComponent().getNormalize());
        Vertex3D b = getRotation() * EngineSetup::get()->up;

        const float theta = newRot.X() * b;
        M3 rotation = M3::getMatrixRotationForEulerAngles(0, 0, theta * this->rotationToTargetSpeed);
        setRotation(getRotation() * rotation.getTranspose());
    }

    if (selectedObject == nullptr) {
        Vertex3D end = getPosition() + Vertex3D(0, 1, 0);
        Vector3D way(getPosition(), end);

        M3 newRot = M3::getFromVectors(EngineSetup::get()->forward, way.getComponent().getNormalize());
        Vertex3D b = getRotation() * EngineSetup::get()->up;

        const float theta = newRot.X() * b;
        M3 rotation = M3::getMatrixRotationForEulerAngles(0, 0, theta * this->rotationToTargetSpeed);
        setRotation(getRotation() * rotation.getTranspose());
    }

    if (counterDamageBlink.isEnabled()) {
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            stopBlinkForPlayer();
        }
    }

    setPosition(getPosition() + this->velocity);

    light->setPosition(getPosition() + Vertex3D(0, 0, -5000));

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
}

Vertex3D Player::getVelocity() {
    return this->velocity;
}

void Player::applyFriction() {
    if (Tools::isZeroVector(this->velocity)) {
        return;
    }

    velocity = velocity + velocity.getInverse().getScaled(Brakeza3D::get()->getDeltaTime() * friction);
}

void Player::setVelocity(Vertex3D v) {
    this->velocity = v;
}

void Player::updateLight() {
    light->setPosition(getPosition() + lightPositionOffset);
    light->onUpdate();
}

void Player::resolveCollision(Collisionable *with)
{
    Mesh3DGhost::resolveCollision(with);
    auto projectile = dynamic_cast<AmmoProjectileBody*> (with);
    if (projectile != nullptr) {
        if (projectile->getParent() != this) {
            if (takeDamage(projectile->getWeaponType()->getDamage())) {
                ComponentSound::playSound(
                    ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("playerDamage"),
                    1,
                    0
               );
            }
            projectile->remove();
        }
    }

    auto laser = dynamic_cast<ProjectileRay*> (with);
    if (laser != nullptr) {
        if (takeDamage(laser->getDamage())) {
            ComponentSound::playSound(
                ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("playerDamage"),
                1,
                0
            );
        }
    }

    auto weapon = dynamic_cast<ItemWeaponGhost*> (with);
    if (weapon != nullptr) {
        this->addWeapon(weapon->getWeaponType());
        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("itemWeapon"),
            1,
            0
        );
        Logging::Log("Added Weapon to Player: %s", weapon->getWeaponType()->getLabel().c_str());
        weapon->setRemoved(true);
    }

    auto health = dynamic_cast<ItemHealthGhost*> (with);
    if (health != nullptr) {
        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("itemHealth"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        Logging::Log("Health to Player!");
        receiveAid(health->getAid());
        health->remove();
    }

    auto energy = dynamic_cast<ItemEnergyGhost*> (with);
    if (energy != nullptr) {
        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("itemHealth"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        Logging::Log("Health to Player!");
        receiveEnergy(energy->getEnergy());
        energy->remove();
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

    ComponentSound::playSound(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("switchWeapon"),
        EngineSetup::SoundChannels::SND_GLOBAL,
        0
    );
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

void Player::setAutoRotationToFacingSelectedObjectSpeed(float value) {
    Player::rotationToTargetSpeed = value;
}

void Player::startBlinkShaderForPlayer()
{
    counterDamageBlink.setEnabled(true);
    blink->setEnabled(true);
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

void Player::setEnergyShieldEnabled(bool shieldEnabled) {
    Player::energyShieldEnabled = shieldEnabled;
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

int Player::getStartStamina() const {
    return startStamina;
}

void Player::setStartStamina(int value) {
    Player::startStamina = value;
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

int Player::getGravityShieldsNumber() const {
    return gravityShieldsNumber;
}

void Player::setGravityShieldsNumber(int value) {
    Player::gravityShieldsNumber = value;
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
        if (weapon->isAvailable()) {
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

void Player::setAllowGravitationalShields(bool value) {
    Player::allowMakeReflections = value;
}

void Player::setAllowEnergyShield(bool value) {
    Player::allowEnergyShield = value;
}

bool Player::isAllowedMakeReflections() const
{
    return allowMakeReflections;
}

bool Player::isAllowEnergyShield() const
{
    return allowEnergyShield;
}

void Player::loadShaders()
{
    blink = new ShaderBlink(false, this, 0.05, Color::green());

    counterDamageBlink.setEnabled(false);
    reflection.loadBlinkShader();
}

PlayerState Player::getState() const
{
    return state;
}

void Player::makeStuck(float time)
{
    counterStucked.setStep(time);
    counterStucked.setEnabled(true);

    setStucked(true);

    velocity = Vertex3D(0, 0, 0);

    setState(PlayerState::GETTING_DAMAGE);

    ComponentSound::playSound(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("electricStuck"),
        EngineSetup::SoundChannels::SND_GLOBAL,
        0
    );

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
    this->light->setEnabled(value);
}

void Player::updateWeaponInteractionStatus() const
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
        ComponentSound::stopChannel(EngineSetup::SND_LASER);
        getWeapon()->setStatus(NONE);
    }
}

RayLight &Player::getShaderLaser()
{
    return rayLight;
}

LightPoint3D *Player::getLight() const {
    return light;
}

Mesh3D *Player::getShieldModel() {
    return &shieldModel;
}

PlayerReflection *Player::getReflection() {
    return &reflection;
}

Player::~Player()
{
    delete light;
    delete blink;

    for (auto w : weaponTypes) {
        delete w;
    }
}
