
#include "../../src/Game/Player.h"
#include "../Brakeza3D.h"
#include "../EngineBuffers.h"
#include "Game.h"

Player::Player() : defaultLives(5), oxygen(100), state(PlayerState::GAMEOVER), dead(false), stamina(EngineSetup::getInstance()->GAME_PLAYER_STAMINA_INITIAL), lives(defaultLives), tookDamage(false), stooped(false)
{
    this->counterStep       = new Counter(0.30);
    this->counterTakeDamage = new Counter(0.10);
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
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerDead" + std::to_string(rndPlayerDead)), EngineSetup::SoundChannels::SND_PLAYER, 0);
    }

    this->dead = dead;
}

void Player::evalStatusMachine()
{
    this->tookDamage = false;

    this->counterStep->update();
    this->counterTakeDamage->update();

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
    this->tookDamage = true;

    if (counterTakeDamage->isFinished()) {
        counterTakeDamage->setEnabled(true);
        int rndPlayerPain = Tools::random(1, 4);
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerPain" + std::to_string(rndPlayerPain)), EngineSetup::SoundChannels::SND_PLAYER, 0);
        Logging::getInstance()->Log("Sound player damage");
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
    SDL_SetRelativeMouseMode(SDL_TRUE);
    Game::get()->initBSP();
}

void Player::respawn()
{
    Brakeza3D::get()->setCameraInBSPStartPosition();
    setDead(false);
    state = PlayerState::LIVE;
    setStamina(100);
    oxygen = 100;
    Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("startGame"), EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
}

void Player::shoot()
{
    for (int i = 0; i < Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getDispersion(); i++) {

        Projectile3DBody *projectile = new Projectile3DBody();
        projectile->setPosition(*Brakeza3D::get()->getCamera()->getPosition());
        projectile->getPosition()->x += i * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) / 5;
        projectile->getPosition()->y += i * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) / 5 ;
        projectile->getPosition()->z += i * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) / 5;
        projectile->setLabel("projectile");
        projectile->setEnabled(true);
        projectile->linkTexturesTo(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getProjectileTemplate());
        projectile->setAnimation(0);
        projectile->makeProjectileRigidBody(
            1,
            Vertex3D(0.5, 0.5, 0.5),
            Brakeza3D::get()->getSceneObjects(),
            Brakeza3D::get()->getCamera(),
            Brakeza3D::get()->getCollisionManager()->getDynamicsWorld(),
            true,
            Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getSpeed(),
            Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getAccuracy()
        );
        projectile->getBillboard()->setDimensions(
                Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->projectileWidth,
                Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->projectileHeight
        );

        projectile->setRotation(Brakeza3D::get()->getCamera()->getRotation());
    }

    int currentWeaponAmmo = Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getAmmoType()->getAmount();
    Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getAmmoType()->setAmount(currentWeaponAmmo - 1);
}


void Player::respawnNPCS()
{
    std::vector<Object3D *>::iterator it;
    for (it = Brakeza3D::get()->getSceneObjects().begin(); it != Brakeza3D::get()->getSceneObjects().end(); it++) {
        Object3D *object = *(it);
        auto *enemy = dynamic_cast<NPCEnemyBody *> (object);

        if (enemy != NULL) {
            enemy->respawn();
        }
    }
}

bool Player::isCrouch() const {
    return stooped;
}

void Player::setCrouch(bool stooped) {
    Player::stooped = stooped;
}

float Player::getOxygen() const {
    return oxygen;
}

void Player::setOxygen(float air) {
    if (air < 0) {
        this->takeDamage(this->stamina);
        return;
    }
    Player::oxygen = air;
}

void Player::getAid(float aid)
{
    this->stamina = stamina + aid;

    if (stamina > EngineSetup::getInstance()->GAME_PLAYER_STAMINA_INITIAL) {
        this->stamina = EngineSetup::getInstance()->GAME_PLAYER_STAMINA_INITIAL;
    }
}
