
#include "../../src/Game/Player.h"
#include "../Brakeza3D.h"

Player::Player() : state(PlayerState::GAMEOVER), dead(false),
                   stamina(INITIAL_STAMINA), lives(INITIAL_LIVES),
                   stopped(false), power(INITIAL_POWER), friction(INITIAL_FRICTION), maxVelocity(INITIAL_MAX_VELOCITY) {

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
    if (this->dead != dead && dead) {
        ComponentsManager::get()->getComponentInput()->setEnabled(false);
    }

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
            EngineSetup::get()->MENU_ACTIVE = true;
        }
    }
}

void Player::newGame() {
    ComponentsManager::get()->getComponentInput()->setEnabled(true);
    setLives(INITIAL_LIVES);
    //SDL_SetRelativeMouseMode(SDL_TRUE);
    EngineSetup::get()->MENU_ACTIVE = false;
    EngineSetup::get()->DRAW_HUD = true;

    this->state = PlayerState::LIVE;
}

void Player::respawn() {
    if (Tools::isValidVector(
            Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->getPosition())) {
    }

    ComponentsManager::get()->getComponentInput()->setEnabled(true);
    setDead(false);
    state = PlayerState::LIVE;
    setStamina(INITIAL_STAMINA);
}

void Player::shoot() {
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

    if (Tools::isZeroVector(this->velocity)) {
        return;
    }

    applyFriction();

    setPosition(getPosition() + this->velocity );
}

Vertex3D Player::getVelocity() {
    return this->velocity;
}

void Player::applyFriction() {
    velocity = velocity + velocity.getInverse().getScaled(Brakeza3D::get()->getDeltaTime() * friction);
}

void Player::setVelocity(Vertex3D v) {
    this->velocity = v;
}
