
#include "Player.h"
#include "../include/Brakeza3D.h"
#include "Weapons/AmmoProjectileBody.h"
#include "Items/ItemWeaponGhost.h"
#include "Items/ItemHealthGhost.h"
#include "Items/ItemEnergyGhost.h"
#include "Items/PlayerReflection.h"
#include "Items/LivingObject.h"
#include "Common/ShockWave.h"
#include "Weapons/WeaponRayLight.h"

Player::Player()
:
    god(false),
    LivingObject(this),
    RotatableToTarget(nullptr, this, 2.0f),
    energy(0),
    startEnergy(0),
    recoverEnergySpeed(INITIAL_RECOVER_ENERGY),
    stuck(false),
    rescuedHumans(0),
    coins(0),
    projectileStartOffsetPosition(1.3),
    weapon(nullptr),
    counterStucked(Counter(5)),
    counterDashCadence(Counter(1)),
    energyShieldEnabled(false),
    allowEnergyShield(false),
    counterLight(Counter(0.05)),
    lightPositionOffset(Vertex3D(0, 0, -1)),
    particlesEngineLeftOffset(Vertex3D(0, 0.2, 0)),
    state(PlayerState::EMPTY),
    satellite(PlayerSatellite(this)),
    avatar(new Image(EngineSetup::get()->ICONS_FOLDER + "avatars/default.png")),
    dashPower(INITIAL_POWER_DASH),
    power(0),
    friction(INITIAL_FRICTION),
    maxVelocity(1),
    velocity(Vertex3D(0, 0, 0))
{
    setTransparent(false);
    setMultiScene(true);

    light = LightPoint3D::base();
    light->setLabel("PlayerLightPoint");
    light->setMultiScene(true);
    light->setRotation(180, 0, 0);
    light->setEnabled(false);
    attachObject(light);

    particleEngineLeft = new ParticleEmitter(
        ParticleEmitterState::DEFAULT,
        nullptr,
        position,
        9999,
        PaletteColors::getParticlesPlayerFrom(),
        PaletteColors::getParticlesPlayerTo(),
        OCParticlesContext::forPlayerEngine(),
        ComponentsManager::get()->getComponentGame()->getImages()->getTextureByLabel("particle01")
    );

    spriteEnergyShield = new Sprite2D(
        0, 0,
        false,
        new TextureAnimated(std::string(EngineSetup::get()->SPRITES_FOLDER + "shield.png"), 180, 180, 20, 60)
    );

    spriteEnergyShield->setEnabled(false);
    spriteEnergyShield->setMultiScene(true);
    spriteEnergyShield->setLabel("playerSpriteEnergyShield");
    attachObject(spriteEnergyShield);

    damages.push_back(new Image(EngineSetup::get()->IMAGES_FOLDER + "blood_splatter_01.png"));
    damages.push_back(new Image(EngineSetup::get()->IMAGES_FOLDER + "blood_splatter_02.png"));
    damages.push_back(new Image(EngineSetup::get()->IMAGES_FOLDER + "blood_splatter_03.png"));
}

void Player::loadSatellite()
{
    satellite.setEnabled(false);
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
    dmg *= componentGame->getLevelLoader()->difficultyRatio;

    if (componentGame->getGameState() != EngineSetup::GAMING) return false;

    if ((state == PlayerState::GETTING_DAMAGE && !isStucked()) || state == PlayerState::DEAD) {
        return false;
    }

    if (!god) this->stamina -= dmg;

    if (stamina <= 0) {
        stamina = 0;
        setState(PlayerState::DEAD);

        Tools::makeFadeInSprite(getPosition(), ComponentsManager::get()->getComponentGame()->getRandomExplosionSprite()->getAnimation());
        ComponentsManager::get()->getComponentSound()->sound("enemyExplosion", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        setEnabled(false);
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

    setStamina(getStartStamina());
    if (ComponentsManager::get()->getComponentGame()->getStoreManager()->isItemEnabled(EngineSetup::StoreItems::ITEM_SATELLITE)) {
        satellite.setEnabled(true);
    }
}

void Player::shoot(float intensity)
{
    if (getWeapon() == nullptr || !getWeapon()->isAvailable()) {
        return;
    }

    bool wasShoot = weapon->shoot({
        AxisUp().getInverse(),
        projectileStartOffsetPosition,
        EngineSetup::collisionGroups::Projectile,
        EngineSetup::collisionGroups::Enemy,
        true
    });

    if (wasShoot) {
        getWeaponLight()->setColor(PaletteColors::getPlayerProjectileLight());
        initLight();
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

    if (counterDamageBlink.isEnabled()) {
        shiftCamera();
        auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();
        float progress = counterDamageBlink.currentPercentage() / 100;
        randomDamage()->drawFlatAlpha(0, 0, 1 - progress, fb);
        
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            stopBlinkForPlayer();
        }
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
    particleEngineLeft->onUpdate();
    particleEngineLeft->setPosition(getPosition() + particlesEngineLeftOffset - AxisUp().getScaled(-1));
    particleEngineLeft->setRotation(getRotation());
}

void Player::postUpdate()
{
    if (!isEnabled()) return;

    Mesh3D::postUpdate();

    if (state == PlayerState::DEAD) {
        return;
    }

    auto game = ComponentsManager::get()->getComponentGame();

    if (game->getStoreManager()->isItemEnabled(EngineSetup::StoreItems::ITEM_SATELLITE)) {
        satellite.postUpdate();
    }

    particleEngineLeft->postUpdate();

    if (weapon != nullptr) {
        auto weaponRay = dynamic_cast<WeaponRayLight*>(weapon);
        if (weaponRay != nullptr && weapon->getStatus() == PRESSED) {
            ComponentsManager::get()->getComponentSound()->sound("projectileRaylight", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }

        for ( auto w: weapons) {
            w->onUpdate();
        }
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
    light->setPosition(getPosition() + lightPositionOffset + AxisUp().getScaled(-0.5));
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
    for (auto w: weapons) {
        w->setEnabled(false);
    }
    weapon->setEnabled(true);
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

    this->weapons.emplace_back(weaponType);
}

Weapon *Player::getWeaponTypeByLabel(const std::string& label)
{
    for (auto & weaponType : this->weapons) {
        if (weaponType->getLabel() == label) {
            return weaponType;
        }
    }

    return nullptr;
}

void Player::setWeaponTypeByIndex(int i) {
    setWeapon(weapons[i]);
}

void Player::stopBlinkForPlayer()
{
    setState(PlayerState::LIVE);
    counterDamageBlink.setEnabled(false);
    blink->setEnabled(false);
}

const std::vector<Weapon *> &Player::getWeapons() const {
    return weapons;
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

void Player::nextWeapon()
{
    auto currentWeapon = getWeapon();
    auto currentIterator = std::find(weapons.begin(), weapons.end(), currentWeapon);
    weapon->setStatus(RELEASED);

    for (auto it = currentIterator; it != weapons.end(); it++) {
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
    auto currentIterator = std::find(weapons.begin(), weapons.end(), currentWeapon);
    weapon->setStatus(RELEASED);

    int index = (int) std::distance(weapons.begin(), currentIterator);

    for (int i = index; i >= 0; i--) {
        if (weapons[i] == currentWeapon) {
            continue;
        }
        auto weapon = weapons[i];
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
    blink = new FXBlink(false, this, 0.05f, PaletteColors::getPlayerDamageBlink());
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

LightPoint3D *Player::getWeaponLight() const {
    return light;
}

Player::~Player()
{
    delete light;
    delete blink;
    delete avatar;
    delete particleEngineLeft;

    for (auto w : weapons) {
        delete w;
    }
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

        Brakeza3D::get()->addObject3D(new ShockWave(getPosition(), 0.50, 1.5, ShockWaveParams::standard(), true), Brakeza3D::uniqueObjectLabel("shockWave"));

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

int Player::getRescuedHumans() const {
    return rescuedHumans;
}

void Player::makeRandomPlayerDamageSound()
{
    std::string randomMetalSound = "metalHit0" + std::to_string(Tools::random(1, 5));

    ComponentsManager::get()->getComponentSound()->sound(randomMetalSound, EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

void Player::initLight()
{
    counterLight.setEnabled(true);
    light->setEnabled(true);
}

void Player::updateSpriteEnergyShield()
{
    if (!spriteEnergyShield->isEnabled()) return;

    Point2D P1 = Transforms::WorldToPoint(getPosition());

    spriteEnergyShield->updatePosition(P1.x, P1.y);
}

void Player::drawImGuiProperties()
{
    Mesh3DGhost::drawImGuiProperties();

    ImGui::Separator();

    if (ImGui::TreeNode("Player Settings")) {

        if (ImGui::TreeNode("Satellite")) {
            satellite.drawImGuiProperties();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Weapon")) {
            getWeapon()->drawImGuiProperties();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("ParticlesEngine")) {
            particleEngineLeft->drawImGuiProperties();

            ImGui::Separator();

            if (ImGui::TreeNode("Position Offset##")) {
                const float range_color_sensibility = 0.01f;
                const float range_col_min = -100.0f;
                const float range_col_max = 100.0f;

                ImGui::DragScalar("x", ImGuiDataType_Float, &particlesEngineLeftOffset.x, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
                ImGui::DragScalar("y", ImGuiDataType_Float, &particlesEngineLeftOffset.y, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
                ImGui::DragScalar("z", ImGuiDataType_Float, &particlesEngineLeftOffset.z, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        ImGui::Separator();
        if (ImGui::TreeNode("Light Offset##")) {
            const float range_color_sensibility = 0.01f;
            const float range_col_min = -100.0f;
            const float range_col_max = 100.0f;

            ImGui::DragScalar("x", ImGuiDataType_Float, &lightPositionOffset.x, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::DragScalar("y", ImGuiDataType_Float, &lightPositionOffset.y, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::DragScalar("z", ImGuiDataType_Float, &lightPositionOffset.z, range_color_sensibility,&range_col_min, &range_col_max, "%f", 1.0f);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void Player::shiftCamera()
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto p = camera->getPosition();
    float intensity = 0.01f;

    auto random = Vertex3D(
        (float) Tools::random(-1, 1) * intensity,
        (float) Tools::random(-1, 1) * intensity,
        0
    );

    camera->setPosition(p + random);
}


Image *Player::randomDamage()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    const int index = std::rand() % damages.size();
    return damages[index];
}

void Player::setMaxVelocity(float maxVelocity) {
    Player::maxVelocity = maxVelocity;
}

void Player::setStartEnergy(float startEnergy) {
    Player::startEnergy = startEnergy;
}

void Player::setPower(float power) {
    Player::power = power;
}
