
#include <SDL2/SDL_image.h>

#include <utility>
#include "../../include/Game/WeaponType.h"
#include "../../include/Brakeza3D.h"

WeaponType::WeaponType() : available(true) {
    this->status = WeaponsActions::IDLE;
    this->damage = 0;
    this->accuracy = 100;
    this->damageRadius = 0;
    this->ammoType = 0;
    this->speed = 1;
    this->available = true;
    this->dispersion = 0;
    this->model = new Mesh3D();
    this->counterCadence = new Counter();
    this->iconHUD = SDL_CreateRGBSurface(0, 100, 100, 32, 0, 0, 0, 0);
}

WeaponType::WeaponType(const std::string& label) {
    this->status = WeaponsActions::IDLE;
    this->label = label;
    this->iconHUD = SDL_CreateRGBSurface(0, 100, 100, 32, 0, 0, 0, 0);
    this->model = new Mesh3D();
    this->counterCadence = new Counter();
}

void WeaponType::onUpdate() {

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

void WeaponType::loadIconHUD(const std::string& file) {
    std::string path = EngineSetup::get()->WEAPONS_FOLDER + file;

    this->iconHUD = IMG_Load(path.c_str());
}

bool WeaponType::isSniper() const {
    return sniper;
}

void WeaponType::setSniper(bool sniper) {
    WeaponType::sniper = sniper;
}

void WeaponType::loadSniperHUD(const std::string& file) {
    std::string path = EngineSetup::get()->WEAPONS_FOLDER + this->label + "/" + file;

    this->sniperHUD = IMG_Load(path.c_str());
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

void WeaponType::shoot() {
    Logging::Log("WeaponType shoot!", "ComponentWeapons");

    auto *weaponsManager = ComponentsManager::get()->getComponentWeapons();
    auto *componentGame = ComponentsManager::get()->getComponentGame();
    auto *componentRender = ComponentsManager::get()->getComponentRender();

    auto ammoType = getAmmoType();
    if (getAmmoType()->getAmount() <= 0) return;

    auto *projectile = new Projectile3DBody();
    projectile->copyFrom(getAmmoType()->getModelProjectile());
    projectile->setPosition( componentGame->getPlayer()->getPosition() + Vertex3D(0, -1000, 0));
    projectile->setLabel("projectile_" + componentRender->getUniqueGameObjectLabel());
    projectile->setEnabled(true);
    projectile->makeProjectileRigidBody(
        0.1,
        Vertex3D(0, -1, 0),
        (float) getSpeed(),
        getAccuracy(),
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()
    );

    Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());

    int currentWeaponAmmo = weaponsManager->getCurrentWeaponType()->getAmmoType()->getAmount();
    weaponsManager->getCurrentWeaponType()->getAmmoType()->setAmount(currentWeaponAmmo - 1);
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
