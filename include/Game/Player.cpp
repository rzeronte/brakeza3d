
#include "../../src/Game/Player.h"
#include "../Brakeza3D.h"
#include "../Particles/ParticleEmissorGravity.h"
#include "AmmoProjectileGhost.h"
#include "ItemWeaponGhost.h"

Player::Player() : state(PlayerState::EMPTY),
                   stamina(INITIAL_STAMINA), lives(INITIAL_LIVES),
                   power(INITIAL_POWER), friction(INITIAL_FRICTION), maxVelocity(INITIAL_MAX_VELOCITY) {

    engineParticles = new ParticleEmissorGravity(true, 120, 10, 0.05, Color::gray());
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

    this->counterDamage = new Counter(3);
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
    this->stamina -= dmg;

    if (state != PlayerState::GETTING_DAMAGE) {
        setState(PlayerState::GETTING_DAMAGE);
        counterDamage->setEnabled(true);
        startBlinkForPlayer();
    }

    if (stamina <= 0) {
        setState(PlayerState::DEAD);
        ComponentsManager::get()->getComponentGame()->setGameState(EngineSetup::GameState::MENU);
        lives--;

        if (lives <= 0) {
            ComponentsManager::get()->getComponentGame()->setGameState(EngineSetup::GameState::MENU);
        }
    }
}

void Player::respawn() {
    Logging::getInstance()->Log("respawn");

    setState(PlayerState::LIVE);
    setStamina(INITIAL_STAMINA);
}

void Player::shoot() {
    if (weaponType == nullptr) {
        return;
    }

    Logging::Log("ComponentWeapons shoot!", "Weapons");

    weaponType->shoot(
        this,
        getPosition() - AxisUp().getScaled(1000),
        AxisUp().getInverse()
    );
}

void Player::jump() {
}

void Player::reload() {
    if (weaponType == nullptr) {
        return;
    }

    Logging::Log("ComponentWeapons reload!", "Weapons");
}

void Player::getAid(float aid) {
    this->stamina = stamina + aid;

    if (stamina > (float) INITIAL_STAMINA) {
        this->stamina = (float) INITIAL_STAMINA;
    }
}

void Player::onUpdate() {
    Mesh3D::onUpdate();

    updateEngineParticles();
    updateLight();
    updateWeaponType();
    applyFriction();

    if (state == PlayerState::GETTING_DAMAGE) {
        counterDamage->update();
        if (counterDamage->isFinished()) {
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
    auto projectile = dynamic_cast<AmmoProjectileGhost*> (with);
    if (projectile != nullptr) {
        if (projectile->getParent() != this) {
            takeDamage(projectile->getWeaponType()->getDamage());
            projectile->remove();
        }
    }

    auto weapon = dynamic_cast<ItemWeaponGhost*> (with);
    if (weapon != nullptr) {
        this->addWeaponType(weapon->getWeaponType());
        Logging::getInstance()->Log("Added Weapon to Player:" + weapon->getWeaponType()->getLabel());
        weapon->removeCollisionObject();
        weapon->setRemoved(true);
    }
}

void Player::setState(PlayerState state) {
    Player::state = state;
}

WeaponType *Player::getWeaponType() const {
    return weaponType;
}

void Player::setWeaponType(WeaponType *weaponType) {
    Player::weaponType = weaponType;
}

void Player::updateWeaponType() {
    if (weaponType != nullptr) {
        weaponType->onUpdate();
    }
}

void Player::createWeaponType(const std::string& label) {
    this->weaponTypes.emplace_back(new WeaponType(label));
}

void Player::addWeaponType(WeaponType *weaponType)
{
    auto weapon = getWeaponTypeByLabel(weaponType->getLabel());
    if (weapon != nullptr) {
        weapon->addAmount(weaponType->getAmmoAmount());
        Logging::getInstance()->Log("Weapon already exist! Added ammo: " + std::to_string(weaponType->getAmmoAmount()));
        return;
    }

    this->weaponTypes.emplace_back(weaponType);
}

WeaponType *Player::getWeaponTypeByLabel(const std::string& label) {
    for (auto & weaponType : this->weaponTypes) {
        if (weaponType->label == label) {
            return weaponType;
        }
    }

    return nullptr;
}

void Player::setWeaponTypeByIndex(int i) {
    if (weaponTypes.size() > i) {
        setWeaponType(weaponTypes[i]);
    } else {
        Logging::getInstance()->Log("Weapon not available(" + std::to_string(i) + ")!");
    }
}

void Player::setAutoRotationSelectedObjectSpeed(float autoRotationSelectedObjectSpeed) {
    Player::autoRotationSelectedObjectSpeed = autoRotationSelectedObjectSpeed;
}

void Player::startBlinkForPlayer() {
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

void Player::setKillsCounter(int killsCounter) {
    Player::killsCounter = killsCounter;
}

void Player::increaseKills() {
    killsCounter++;
}

int Player::getLevelCompletedCounter() const {
    return levelsCompletedCounter;
}

void Player::setLevelCompletedCounter(int levelCounter) {
    Player::levelsCompletedCounter = levelCounter;
}

void Player::increaseLevelsCompleted() {
    levelsCompletedCounter++;
}
