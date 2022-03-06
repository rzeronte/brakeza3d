
#include "../../src/Game/Player.h"
#include "../Brakeza3D.h"
#include "../Particles/ParticleEmissorGravity.h"

Player::Player() : state(PlayerState::GAMEOVER), dead(false),
                   stamina(INITIAL_STAMINA), lives(INITIAL_LIVES),
                   stopped(false), power(INITIAL_POWER), friction(INITIAL_FRICTION), maxVelocity(INITIAL_MAX_VELOCITY) {

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

bool Player::isDead() const {
    return dead;
}

void Player::setDead(bool dead) {
    this->dead = dead;
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
    if (dead) return;

    this->stamina -= dmg;

    if (stamina <= 0) {
        state = PlayerState::DEAD;
        setDead(true);
        lives--;

        if (lives <= 0) {
            state = PlayerState::GAMEOVER;
            ComponentsManager::get()->getComponentGame()->setGameState(GameState::MENU);
        }
    }
}

void Player::newGame() {
    setLives(INITIAL_LIVES);
    //SDL_SetRelativeMouseMode(SDL_TRUE);
    ComponentsManager::get()->getComponentGame()->setGameState(GameState::GAMING);
    EngineSetup::get()->DRAW_HUD = true;

    this->state = PlayerState::LIVE;
}

void Player::respawn() {
    if (Tools::isValidVector(
            Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->getPosition())) {
    }

    setDead(false);
    state = PlayerState::LIVE;
    setStamina(INITIAL_STAMINA);
}

void * Player::shoot() {
    Brakeza3D::get()->getComponentsManager()->getComponentWeapons()->shoot();
}

void Player::jump() {
}

void Player::reload() {
    Brakeza3D::get()->getComponentsManager()->getComponentWeapons()->reload();
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
    applyFriction();

    auto selectedObject = ComponentsManager::get()->getComponentRender()->getSelectedObject();
    if (selectedObject != nullptr) {
        Vector3D way(selectedObject->getPosition(), getPosition());
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
    Mesh3DGhost::resolveCollision(with);
    //Logging::getInstance()->Log("Collision specific for Player");
}
