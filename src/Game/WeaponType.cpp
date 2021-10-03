
#include <SDL2/SDL_image.h>

#include <utility>
#include "../../headers/Game/WeaponType.h"
#include "../../headers/Brakeza3D.h"

WeaponType::WeaponType() : available(true) {
}

WeaponType::WeaponType(const std::string& label) {
    this->label = label;
    this->iconHUD = SDL_CreateRGBSurface(0, 100, 100, 32, 0, 0, 0, 0);

    weaponAnimations = new Mesh3DAnimatedCollection();
    weaponAnimations->setFollowCamera(true);
    weaponAnimations->setLabel("weapon_" + label);
    weaponAnimations->setEnabled(false);
    weaponAnimations->rotationFixed = M3::getMatrixRotationForEulerAngles(90, 0, 0);
    Brakeza3D::get()->addObject3D(weaponAnimations, weaponAnimations->getLabel());

    model = new Mesh3D();

    counterCadence = new Counter();
}

void WeaponType::addAnimation(std::string newLabel, const std::string& newModel, float scale, bool stopEnd) {
    std::string full_animation_folder = EngineSetup::getInstance()->MODELS_FOLDER + newModel;

    weaponAnimations->addAnimation(std::move(newLabel), newModel, scale, stopEnd);
    weaponAnimations[this->numAnimations].rotationFixed = M3::getMatrixRotationForEulerAngles(90, 0, 180);
    this->numAnimations++;
}

void WeaponType::onUpdate() {
    if (getCurrentWeaponAnimation()->isAnimationEnds()) {
        this->setWeaponAnimation(EngineSetup::WeaponsActions::IDLE);
        this->setFiring(false);
    }
}

void WeaponType::setWeaponAnimation(int animationIndex) {
    this->currentAnimationIndex = animationIndex;

    this->weaponAnimations->setAnimation(animationIndex);

    if (EngineSetup::getInstance()->LOG_WEAPONS_SYSTEM) {
        Logging::Log("setWeaponAnimation: " + std::to_string(animationIndex), "WeaponType");
    }
}

Mesh3DAnimated *WeaponType::getCurrentWeaponAnimation() const {
    return this->weaponAnimations->getCurrentMesh3DAnimated();
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
    std::string path = EngineSetup::getInstance()->WEAPONS_FOLDER + file;

    this->iconHUD = IMG_Load(path.c_str());
}

bool WeaponType::isFiring() const {
    return firing;
}

void WeaponType::setFiring(bool newFiring) {
    WeaponType::firing = newFiring;
}

bool WeaponType::isSniper() const {
    return sniper;
}

void WeaponType::setSniper(bool sniper) {
    WeaponType::sniper = sniper;
}

void WeaponType::loadSniperHUD(const std::string& file) {
    std::string path = EngineSetup::getInstance()->WEAPONS_FOLDER + this->label + "/" + file;

    this->sniperHUD = IMG_Load(path.c_str());
}

bool WeaponType::isSniperEnabled() const {
    return sniperEnabled;
}

void WeaponType::setSniperEnabled(bool sniperEnabled) {
    WeaponType::sniperEnabled = sniperEnabled;
}

const std::string &WeaponType::getClassname() const {
    return classname;
}

void WeaponType::setClassname(const std::string &classname) {
    WeaponType::classname = classname;
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
    return ammo;
}

void WeaponType::setAmmoType(AmmoType *ammo) {
    WeaponType::ammo = ammo;
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
    if (isFiring()) return;

    this->setFiring(true);

    this->setWeaponAnimation(EngineSetup::WeaponsActions::FIRE);

    ComponentWeapons *weaponsManager = Brakeza3D::get()->getComponentsManager()->getComponentWeapons();

    if (getAmmoType()->getAmount() <= 0) return;

    for (int i = 0; i < weaponsManager->getCurrentWeaponType()->getDispersion(); i++) {

        auto *projectile = new Projectile3DBody();
        projectile->copyFrom(weaponsManager->getCurrentWeaponType()->getAmmoType()->getModelProjectile());
        projectile->setFromEnemy(false);
        projectile->setDamage(weaponsManager->getCurrentWeaponType()->getDamage());
        projectile->setDamageRadius(weaponsManager->getCurrentWeaponType()->getDamageRadius());
        projectile->setPosition(
                Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->getPosition());

        Vertex3D dir =
                Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->getRotation().getTranspose() *
                EngineSetup::getInstance()->forward;
        projectile->setPosition(projectile->getPosition() + dir.getScaled(10));
        projectile->getPosition().x += i * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) / 5;
        projectile->getPosition().y += i * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) / 5;
        projectile->getPosition().z += i * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) / 5;
        projectile->setLabel("projectile_" + weaponsManager->getCurrentWeaponType()->getAmmoType()->getName());
        projectile->setEnabled(true);
        projectile->makeProjectileRigidBody(
                1,
                Vertex3D(0.5, 0.5, 0.5),
                Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(),
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                true,
                weaponsManager->getCurrentWeaponType()->getSpeed(),
                weaponsManager->getCurrentWeaponType()->getAccuracy()
        );

        projectile->setRotation(
                Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera()->getRotation().getTranspose());
        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());
    }

    int currentWeaponAmmo = weaponsManager->getCurrentWeaponType()->getAmmoType()->getAmount();
    weaponsManager->getCurrentWeaponType()->getAmmoType()->setAmount(currentWeaponAmmo - 1);

    // Sound projectile
    const std::string soundLabel = weaponSounds[this->getCurrentWeaponAnimation()->indexCurrentAnimation];

    Tools::playMixedSound(
            EngineBuffers::getInstance()->soundPackage->getSoundByLabel(soundLabel),
            EngineSetup::SoundChannels::SND_WEAPON,
            0
    );
}

void WeaponType::reload() {
    if (getAmmoType()->getReloads() > 0) {
        getAmmoType()->setAmount(getAmmoType()->getReloadAmount());
        getAmmoType()->setReloads(getAmmoType()->getReloads() - 1);

        this->setWeaponAnimation(EngineSetup::WeaponsActions::RELOAD);

        const std::string soundLabel = weaponSounds[EngineSetup::WeaponsActions::RELOAD];
        Tools::playMixedSound(
                EngineBuffers::getInstance()->soundPackage->getSoundByLabel(soundLabel),
                EngineSetup::SoundChannels::SND_WEAPON,
                0
        );
    }
}

Mesh3DAnimatedCollection *WeaponType::getWeaponAnimations() const {
    return weaponAnimations;
}

void WeaponType::setWeaponAnimations(Mesh3DAnimatedCollection *weaponAnimations) {
    WeaponType::weaponAnimations = weaponAnimations;
}

std::vector<std::string> &WeaponType::getWeaponSounds() {
    return weaponSounds;
}

void WeaponType::setWeaponSounds(const std::vector<std::string> &weaponSounds) {
    WeaponType::weaponSounds = weaponSounds;
}

const std::string &WeaponType::getSoundEmptyLabel() const {
    return soundEmptyLabel;
}

void WeaponType::setSoundEmptyLabel(const std::string &soundEmptyLabel) {
    WeaponType::soundEmptyLabel = soundEmptyLabel;
}

