
#include <SDL2/SDL_image.h>

#include "../../include/Game/WeaponType.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Game/AmmoProjectileGhost.h"

WeaponType::WeaponType(const std::string& label) {
    this->status = WeaponsActions::IDLE;
    this->damage = 0;
    this->accuracy = 100;
    this->damageRadius = 0;
    this->ammoType = 0;
    this->speed = 1;
    this->available = true;
    this->dispersion = 0;
    this->label = label;
    this->model = new Mesh3D();
    this->cadenceTime = 1;
    this->counterCadence = new Counter(this->cadenceTime);
    this->counterCadence->setEnabled(true);
}

void WeaponType::onUpdate() {
    counterCadence->update();
}

void WeaponType::setSpeed(float speed) {
    this->speed = speed;
}


float WeaponType::getDamage() const {
    return this->damage;
}

void WeaponType::setDamage(float damage) {
    this->damage = damage;
}

float WeaponType::getAccuracy() const {
    return accuracy;
}

void WeaponType::setAccuracy(float accuracy) {
    WeaponType::accuracy = accuracy;
}

int WeaponType::getDispersion() const {
    return dispersion;
}

void WeaponType::setDispersion(float dispersion) {
    WeaponType::dispersion = dispersion;
}

int WeaponType::getSpeed() const {
    return speed;
}


bool WeaponType::isSniper() const {
    return sniper;
}

void WeaponType::setSniper(bool sniper) {
    WeaponType::sniper = sniper;
}

bool WeaponType::isSniperEnabled() const {
    return sniperEnabled;
}

void WeaponType::setSniperEnabled(bool sniperEnabled) {
    WeaponType::sniperEnabled = sniperEnabled;
}

bool WeaponType::isAvailable() const {
    return available;
}

void WeaponType::setAvailable(bool available) {
    WeaponType::available = available;
}

int WeaponType::getIndex() const {
    return index;
}

void WeaponType::setIndex(int index) {
    WeaponType::index = index;
}

AmmoType *WeaponType::getAmmoType() const {
    return ammoType;
}

void WeaponType::setAmmoType(AmmoType *ammo) {
    WeaponType::ammoType = ammo;
}

float WeaponType::getDamageRadius() const {
    return damageRadius;
}

void WeaponType::setDamageRadius(float damageRadius) {
    WeaponType::damageRadius = damageRadius;
}

Mesh3D *WeaponType::getModel() const {
    return model;
}

void WeaponType::shoot(Object3D *parent, Vertex3D position, Vertex3D direction)
{
    const int ammoAmount = getAmmoType()->getAmount();
    if (ammoAmount <= 0) return;

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);

        auto *weaponsManager = ComponentsManager::get()->getComponentWeapons();
        auto *componentRender = ComponentsManager::get()->getComponentRender();

        Logging::Log("WeaponType shoot!", "ComponentWeapons");

        auto *projectile = new AmmoProjectileGhost();
        projectile->setParent(parent);
        projectile->setLabel("projectile_" + componentRender->getUniqueGameObjectLabel());
        projectile->setWeaponType(this);
        projectile->copyFrom(getAmmoType()->getModelProjectile());
        projectile->setPosition( position);
        projectile->setEnabled(true);
        projectile->setTTL(EngineSetup::get()->PROJECTILE_DEMO_TTL);
        projectile->makeProjectileRigidBody(
            0.1,
            direction,
            (float) getSpeed(),
            getAccuracy(),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()
        );

        weaponsManager->getCurrentWeaponType()->getAmmoType()->setAmount(ammoAmount - 1);

        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());
        Tools::playMixedSound(EngineBuffers::getInstance()->soundPackage->getSoundByLabel("bulletWhisper"),EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
    }
}

void WeaponType::reload() {
    if (getAmmoType()->getReloads() > 0) {
        getAmmoType()->setAmount(getAmmoType()->getReloadAmount());
        getAmmoType()->setReloads(getAmmoType()->getReloads() - 1);

        Tools::playMixedSound(
            EngineBuffers::getInstance()->soundPackage->getSoundByLabel(fireSound),
            EngineSetup::SoundChannels::SND_WEAPON,
            0
        );
    }
}

const std::string &WeaponType::getSoundEmptyLabel() const {
    return soundEmptyLabel;
}

void WeaponType::setSoundEmptyLabel(const std::string &label) {
    WeaponType::soundEmptyLabel = label;
}


const std::string &WeaponType::getSoundFire() const {
    return fireSound;
}

void WeaponType::setSoundFire(const std::string &label) {
    WeaponType::fireSound = label;
}

const std::string &WeaponType::getLabel() const {
    return label;
}

float WeaponType::getCadenceTime() const {
    return cadenceTime;
}

void WeaponType::setCadenceTime(float cadenceTime) {
    WeaponType::cadenceTime = cadenceTime;
    this->counterCadence->setStep(cadenceTime);
    this->counterCadence->setEnabled(true);
}
