
#include "Player.h"
#include "../include/Brakeza3D.h"
#include "../include/Particles/ParticleEmissorGravity.h"
#include "src/weapons/AmmoProjectileBody.h"
#include "src/items/ItemWeaponGhost.h"
#include "src/items/ItemHealthGhost.h"
#include "src/items/ItemEnergyGhost.h"
#include "GravitationalShield.h"

Player::Player() : state(PlayerState::EMPTY),
                   stamina(INITIAL_STAMINA),
                   startStamina(INITIAL_STAMINA),
                   energy(INITIAL_ENERGY),
                   startEnergy(INITIAL_ENERGY),
                   recoverEnergySpeed(INITIAL_RECOVER_ENERGY),
                   lives(INITIAL_LIVES),
                   power(INITIAL_POWER),
                   friction(INITIAL_FRICTION),
                   maxVelocity(INITIAL_MAX_VELOCITY),
                   dashPower(INITIAL_POWERDASH)
{
    engineParticles = new ParticleEmissorGravity(true, 120, 10, 0.05, Color::cyan());
    engineParticles->setRotationFrame(0, 4, 5);

    light = new LightPoint3D();
    light->setEnabled(true);
    light->setLabel("lp2");
    light->setPower(200);
    light->setColor(255, 255, 255);
    light->setColorSpecularity(220, 220, 30);
    light->setSpecularComponent(3);
    light->setColor(0, 255, 0);
    light->setRotation(270, 0, 0);

    engineParticlesPositionOffset = Vertex3D(0, 450, 0);
    lightPositionOffset = Vertex3D(0, -550, 0);

    autoRotationSelectedObjectSpeed = 1;
    killsCounter = 0;
    levelsCompletedCounter = 0;

    this->counterDamageBlink = new Counter(0.45);

    this->energyShieldEnabled = false;
    this->gravityShieldsNumber = 0;

    this->currentWeaponIndex = 0;

    setAllowEnergyShield(true);
    setAllowGravitationalShields(true);
}

void Player::loadShieldModel() {
    shieldModel = new Mesh3D();
    shieldModel->setEnabled(false);
    shieldModel->setLabel("shieldModel");
    shieldModel->setPosition(getPosition());
    shieldModel->setAlpha(200);
    shieldModel->setAlphaEnabled(true);
    shieldModel->setEnableLights(true);
    shieldModel->setScale(1);
    shieldModel->setStencilBufferEnabled(false);
    shieldModel->setFlatTextureColor(true);
    shieldModel->setFlatColor(Color::blue());
    shieldModel->setRotationFrameEnabled(true);
    shieldModel->setRotationFrame(Vertex3D(1, 0, 0));
    shieldModel->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "shield.fbx"));

    Brakeza3D::get()->addObject3D(shieldModel, "shieldPlayer");
}


int Player::getStamina() const {
    return stamina;
}

void Player::setStamina(int stamina) {
    Player::stamina = stamina;
}

void Player::setLives(int lives) {
    Player::lives = lives;
}


void Player::evalStatusMachine() {

    switch (state) {
        case PlayerState::LIVE:
            break;
        case PlayerState::DEAD:
            break;
    }
}

void Player::takeDamage(float dmg) {

    if (state == PlayerState::GETTING_DAMAGE || state == PlayerState::DEAD) {
        return;
    }

    if (isEnergyShieldEnabled() && getEnergy() > 0) {
        useEnergy(dmg);
        return;
    }

    this->stamina -= dmg;

    if (state != PlayerState::GETTING_DAMAGE) {
        startPlayerBlink();
    }

    if (stamina <= 0) {
        setState(PlayerState::DEAD);
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("playerDead"),
            1,
            0
        );
        ComponentsManager::get()->getComponentGame()->makeFadeToGameState(EngineSetup::GameState::PRESSKEY_BY_DEAD);
        lives--;
    }
}

void Player::startPlayerBlink()
{
    setState(PlayerState::GETTING_DAMAGE);
    counterDamageBlink->setEnabled(true);
    startBlinkShaderForPlayer();
}

void Player::respawn()
{
    Logging::getInstance()->Log("Respawn Player");
    setState(PlayerState::LIVE);
    setStamina(INITIAL_STAMINA);
}

void Player::makeGravitationalShield()
{
    if (gravityShieldsNumber >= (int) MAX_GRAVITATIONAL_SHIELDS) {
        return;
    }

    auto *object = new GravitationalShield(3000, 0.001, 500, 7);
    object->setParent(parent);
    object->setLabel("talent_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    object->setPosition(getPosition());
    object->setEnabled(true);
    object->setStencilBufferEnabled(true);
    object->setRotationFrameEnabled(true);
    object->setRotationFrame(Vertex3D(0, 0.5, 0));
    object->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "energy_ball.fbx"));
    object->makeGhostBody(
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        object,
        EngineSetup::collisionGroups::Player,
        EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::Projectile
    );

    Brakeza3D::get()->addObject3D(object, object->getLabel());

    gravityShieldsNumber++;

    ComponentsManager::get()->getComponentSound()->playSound(
        EngineBuffers::getInstance()->soundPackage->getByLabel("gravitationalShield"),
        1,
        0
    );
}

void Player::shoot()
{
    if (getWeapon() == nullptr) {
        return;
    }

    if (!getWeapon()->isAvailable()) {
        return;
    }

    const int type = getWeapon()->getType();
    switch(type) {
        case WeaponTypes::WEAPON_PROJECTILE: {
            weapon->shootProjectile(
                this,
                getPosition() - AxisUp().getScaled(1000),
                AxisUp().getInverse(),
                EngineSetup::collisionGroups::Enemy,
                Color::green()
            );
            break;
        }
        case WeaponTypes::WEAPON_INSTANT: {
            auto enemy = dynamic_cast<EnemyGhost*>(ComponentsManager::get()->getComponentRender()->getSelectedObject());
            weapon->shootInstant(
                getPosition(),
                enemy
            );
            break;
        }
        case WeaponTypes::WEAPON_SMART_PROJECTILE: {
            weapon->shootSmartProjectile(
                this,
                getPosition() - AxisUp().getScaled(1000),
                AxisUp().getInverse(),
                EngineSetup::collisionGroups::Enemy,
                nullptr,
                Color::cyan()
            );
        }
    }
}

void Player::receiveAid(float aid) {
    this->stamina = stamina + aid;

    if (stamina > (float) getStartStamina()) {
        this->stamina = (float) getStartStamina();
    }
}
void Player::receiveEnergy(float energy)
{
    this->energy = energy + energy;

    if (energy > (float) getStartEnergy()) {
        this->energy = (float) getStartEnergy();
    }
}

void Player::onUpdate()
{
    Mesh3D::onUpdate();

    updateEngineParticles();
    updateLight();
    updateWeaponType();
    applyFriction();

    if (getEnergy() < getStartEnergy()) {
        setEnergy(std::min(getEnergy() + recoverEnergySpeed, getStartEnergy()));
    }

    if (isEnergyShieldEnabled()) {
        shieldModel->setPosition(getPosition());
        shieldModel->setAlpha( std::clamp((int)shieldModel->getAlpha() + 2, 0, 127));
    } else {
        if (shieldModel->getAlpha() == 0) {
            shieldModel->setEnabled(false);
        }
       shieldModel->setAlpha( std::clamp((int)shieldModel->getAlpha() - 2, 0, 127));
    }

    if (state == PlayerState::GETTING_DAMAGE) {
        counterDamageBlink->update();
        if (counterDamageBlink->isFinished()) {
            setState(PlayerState::LIVE);
            stopBlinkForPlayer();
        }
    }

    auto selectedObject = ComponentsManager::get()->getComponentRender()->getSelectedObject();
    if (selectedObject != this && selectedObject != nullptr) {
        Vector3D way(selectedObject->getPosition(), getPosition());
        M3 newRot = M3::getFromVectors(EngineSetup::get()->forward,way.getComponent().getNormalize());
        Vertex3D b = getRotation() * EngineSetup::get()->up;

        const float theta = newRot.X() * b;
        M3 rotation = M3::getMatrixRotationForEulerAngles(0, 0, theta * this->autoRotationSelectedObjectSpeed);
        setRotation(getRotation() * rotation.getTranspose());
    }

    if (selectedObject == nullptr) {
        Vertex3D end = getPosition() + Vertex3D(0, 1, 0);
        Vector3D way(getPosition(), end);

        M3 newRot = M3::getFromVectors(EngineSetup::get()->forward,way.getComponent().getNormalize());
        Vertex3D b = getRotation() * EngineSetup::get()->up;

        const float theta = newRot.X() * b;
        M3 rotation = M3::getMatrixRotationForEulerAngles(0, 0, theta * this->autoRotationSelectedObjectSpeed);
        setRotation(getRotation() * rotation.getTranspose());
    }

    setPosition(getPosition() + this->velocity );
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

void Player::updateEngineParticles() {
    engineParticles->setPosition(getPosition() + engineParticlesPositionOffset);
    engineParticles->onUpdate();
}

void Player::updateLight() {
    light->setPosition(getPosition() + lightPositionOffset);
    light->onUpdate();
}

void Player::resolveCollision(Collisionable *with) {
    auto *object = dynamic_cast<Object3D*> (with);
    Logging::getInstance()->Log("Player: Collision "  + getLabel() + " with " + object->getLabel());

    Mesh3DGhost::resolveCollision(with);
    auto projectile = dynamic_cast<AmmoProjectileBody*> (with);
    if (projectile != nullptr) {
        if (projectile->getParent() != this) {
            ComponentsManager::get()->getComponentSound()->playSound(
                EngineBuffers::getInstance()->soundPackage->getByLabel("playerDamage"),
                1,
                0
            );
            takeDamage(projectile->getWeaponType()->getDamage());
            projectile->remove();
        }
    }

    auto weapon = dynamic_cast<ItemWeaponGhost*> (with);
    if (weapon != nullptr) {
        this->addWeapon(weapon->getWeaponType());
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("itemWeapon"),
            1,
            0
        );
        Logging::getInstance()->Log("Added Weapon to Player:" + weapon->getWeaponType()->getLabel());
        weapon->removeCollisionObject();
        weapon->setRemoved(true);
    }

    auto health = dynamic_cast<ItemHealthGhost*> (with);
    if (health != nullptr) {
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("itemHealth"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        Logging::getInstance()->Log("Health to Player!");
        receiveAid(health->getAid());
        health->remove();
    }

    auto energy = dynamic_cast<ItemEnergyGhost*> (with);
    if (energy != nullptr) {
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("itemHealth"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        Logging::getInstance()->Log("Health to Player!");
        receiveEnergy(energy->getEnergy());
        energy->remove();
    }

    auto enemy = dynamic_cast<EnemyGhost*> (with);
    if (enemy != nullptr) {
        takeDamage(getStartStamina() / 10 );
    }
}

void Player::setState(PlayerState state) {
    Player::state = state;
}

Weapon *Player::getWeapon() const {
    return weapon;
}

void Player::setWeapon(Weapon *weaponType) {
    Player::weapon = weaponType;
}

void Player::updateWeaponType() {
    if (weapon != nullptr) {
        weapon->onUpdate();
    }
}

void Player::createWeapon(const std::string& label) {
    this->weaponTypes.emplace_back(new Weapon(label));
}

void Player::addWeapon(Weapon *weaponType)
{
    auto weapon = getWeaponTypeByLabel(weaponType->getLabel());
    if (weapon != nullptr) {
        weapon->addAmount(weaponType->getStartAmmoAmount());
        weapon->setAvailable(true);
        setWeapon(weapon);
        Logging::getInstance()->Log("Weapon already exist! Added ammo: " + std::to_string(weaponType->getStartAmmoAmount()));
        return;
    }

    this->weaponTypes.emplace_back(weaponType);
}

Weapon *Player::getWeaponTypeByLabel(const std::string& label) {
    for (auto & weaponType : this->weaponTypes) {
        if (weaponType->label == label) {
            return weaponType;
        }
    }

    return nullptr;
}

void Player::setWeaponTypeByIndex(int i) {
    setWeapon(weaponTypes[i]);
    this->currentWeaponIndex = i;
}

void Player::setAutoRotationToFacingSelectedObjectSpeed(float autoRotationSelectedObjectSpeed) {
    Player::autoRotationSelectedObjectSpeed = autoRotationSelectedObjectSpeed;
}

void Player::startBlinkShaderForPlayer() {
    auto shaderBlink = dynamic_cast<ShaderBlink*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::BLINK));
    shaderBlink->setObject(this);
    shaderBlink->setStep(0.05);
    shaderBlink->setEnabled(true);
}

void Player::stopBlinkForPlayer() {
    auto shaderBlink = dynamic_cast<ShaderBlink*> (ComponentsManager::get()->getComponentRender()->getShaderByType(EngineSetup::ShaderTypes::BLINK));
    shaderBlink->setEnabled(false);
}

int Player::getKillsCounter() const {
    return killsCounter;
}

void Player::setKillsCounter(int killsCounter)
{
    Player::killsCounter = killsCounter;
}

void Player::increaseKills()
{
    killsCounter++;
}

int Player::getLevelCompletedCounter() const
{
    return levelsCompletedCounter;
}

void Player::setLevelCompletedCounter(int levelCounter)
{
    Player::levelsCompletedCounter = levelCounter;
}

void Player::increaseLevelsCompleted()
{
    levelsCompletedCounter++;
}

void Player::decreaseLevelsCompleted()
{
    if (levelsCompletedCounter > 0) {
        levelsCompletedCounter--;
    }
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

void Player::useEnergy(float energy)
{
    this->energy -= energy;
}

void Player::setEnergy(float energy) {
    Player::energy = energy;
}

int Player::getStartStamina() const {
    return startStamina;
}

void Player::setStartStamina(int startStamina) {
    Player::startStamina = startStamina;
}

float Player::getStartEnergy() const {
    return startEnergy;
}

void Player::setStartEnergy(float startEnergy) {
    Player::startEnergy = startEnergy;
}

float Player::getRecoverEnergySpeed() const {
    return recoverEnergySpeed;
}

void Player::setRecoverEnergySpeed(float recoverEnergySpeed) {
    Player::recoverEnergySpeed = recoverEnergySpeed;
}

int Player::getGravityShieldsNumber() const {
    return gravityShieldsNumber;
}

void Player::setGravityShieldsNumber(int gravityShieldsNumber) {
    Player::gravityShieldsNumber = gravityShieldsNumber;
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

void Player::setAllowGravitationalShields(bool allowGravitationalShields) {
    Player::allowGravitationalShields = allowGravitationalShields;
}

void Player::setAllowEnergyShield(bool allowEnergyShield) {
    Player::allowEnergyShield = allowEnergyShield;
}

bool Player::isAllowGravitationalShields() const {
    return allowGravitationalShields;
}

bool Player::isAllowEnergyShield() const {
    return allowEnergyShield;
}
