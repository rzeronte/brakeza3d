
#include "../../src/Game/Player.h"
#include "../Brakeza3D.h"
#include "../Render/EngineBuffers.h"

Player::Player() : defaultLives(2), state(PlayerState::GAMEOVER), dead(false), stamina(100), lives(defaultLives)
{
    this->counterStep = new Counter();
    this->counterStep->setStep(0.40);

    this->counterTakeDamage = new Counter();
    this->counterTakeDamage->setStep(0.75);
}

int Player::getStamina() const {
    return stamina;
}

void Player::setStamina(int stamina) {
    Player::stamina = stamina;
}

int Player::getLives() const {
    return lives;
}

void Player::setLives(int lives) {
    Player::lives = lives;
}

bool Player::isDead() const {
    return dead;
}

void Player::setDead(bool dead)
{
    if (this->dead != dead && dead) {
        int rndPlayerDead = Tools::random(1, 6);
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerDead" + std::to_string(rndPlayerDead)));
    }

    this->dead = dead;
}

void Player::evalStatusMachine()
{
    switch (state) {
        case PlayerState::LIVE:
            break;
        case PlayerState::DEAD:
            break;
    }
}

void Player::takeDamage(float dmg)
{
    if (dead) return;

    this->stamina -= dmg;

    if (counterTakeDamage->isFinished()) {
        int rndPlayerPain = Tools::random(1, 4);
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerPain" + std::to_string(rndPlayerPain)));
    }

    if (stamina <= 0) {
        state = PlayerState::DEAD;
        setDead( true );
        lives--;

        if (lives <= 0) {
            state = PlayerState::GAMEOVER;
            EngineSetup::getInstance()->MENU_ACTIVE = true;
        }
    }
}

void Player::newGame()
{
    setLives(defaultLives);

    this->respawn();

    EngineSetup::getInstance()->MENU_ACTIVE = false;
    EngineSetup::getInstance()->DRAW_WEAPON = true;
    EngineSetup::getInstance()->DRAW_HUD = true;
}

void Player::respawn()
{
    Brakeza3D::get()->setCameraInBSPStartPosition();
    setDead(false);
    state = PlayerState::LIVE;
    setStamina(100);
    Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("startGame"));
}

void Player::shoot()
{
    Projectile3DBody *projectile = new Projectile3DBody();
    projectile->setPosition(*Brakeza3D::get()->getCamera()->getPosition());
    projectile->setLabel("projectile");
    projectile->setEnabled(true);
    projectile->linkTexturesTo(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getProjectileTemplate());
    projectile->setAnimation(0);
    projectile->makeProjectileRigidBody(1, Brakeza3D::get()->getSceneObjects(), Brakeza3D::get()->getCamera(), Brakeza3D::get()->getCollisionManager()->getDynamicsWorld(), true, Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->speed);
    projectile->getBillboard()->setDimensions(
            Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->projectileWidth,
            Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->projectileHeight
    );

    // Reduce ammo for this weapon type
    Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->ammo--;

    // Giramos antes de hacer el rigidbody, para no alterar los cálculos en la dirección
    // del impulso en el interior del makeRigidBody
    projectile->setRotation(Brakeza3D::get()->getCamera()->getRotation());

    // Si ya estábamos disparando, no interrumpimos la animación
    Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->startAction();
    Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->setWeaponAnimation(EngineSetup::getInstance()->WeaponsActions::WEAPON_ACTION_FIRE);

    Tools::playMixedSound(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->soundFire);
}