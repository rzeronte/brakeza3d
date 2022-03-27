
#include "../../include/Game/Weapon.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Game/AmmoProjectileBody.h"
#include "../../include/Game/AmmoSmartProjectileBody.h"

Weapon::Weapon(const std::string& label) {
    this->status = WeaponStatus::IDLE;
    this->damage = 0;
    this->ammoAmount = 0;
    this->accuracy = 100;
    this->damageRadius = 0;
    this->speed = 1;
    this->available = true;
    this->dispersion = 0;
    this->label = label;
    this->model = new Mesh3D();
    this->cadenceTime = 1;
    this->counterCadence = new Counter(this->cadenceTime);
    this->counterCadence->setEnabled(true);
    this->modelProjectile = new Mesh3DBody();
    this->type = WeaponTypes::WEAPON_PROJECTILE;
}

void Weapon::onUpdate() {
    counterCadence->update();
}

void Weapon::setSpeed(float speed) {
    this->speed = speed;
}


float Weapon::getDamage() const {
    return this->damage;
}

void Weapon::setDamage(float damage) {
    this->damage = damage;
}

float Weapon::getAccuracy() const {
    return accuracy;
}

void Weapon::setAccuracy(float accuracy) {
    Weapon::accuracy = accuracy;
}

int Weapon::getDispersion() const {
    return dispersion;
}

void Weapon::setDispersion(float dispersion) {
    Weapon::dispersion = dispersion;
}

int Weapon::getSpeed() const {
    return speed;
}

bool Weapon::isAvailable() const {
    return available;
}

void Weapon::setAvailable(bool available) {
    Weapon::available = available;
}

float Weapon::getDamageRadius() const {
    return damageRadius;
}

void Weapon::setDamageRadius(float damageRadius) {
    Weapon::damageRadius = damageRadius;
}

Mesh3D *Weapon::getModel() const {
    return model;
}

void Weapon::shootProjectile(Object3D *parent, Vertex3D position, Vertex3D direction)
{
    const int ammoAmount = getAmmoAmount();

    if (ammoAmount <= 0) return;

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);

        auto *componentRender = ComponentsManager::get()->getComponentRender();

        Logging::Log("Weapon shootProjectile from " + parent->getLabel(), "ComponentWeapons");

        auto *projectile = new AmmoProjectileBody();
        projectile->setParent(parent);
        projectile->setLabel("projectile_" + componentRender->getUniqueGameObjectLabel());
        projectile->setWeaponType(this);
        projectile->copyFrom(getModelProjectile());
        projectile->setPosition( position );
        projectile->setEnabled(true);
        projectile->setTTL(EngineSetup::get()->PROJECTILE_DEMO_TTL);
        projectile->makeProjectileRigidBody(
                0.1,
                direction,
                (float) getSpeed(),
                getAccuracy(),
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()
        );

        setAmmoAmount(ammoAmount - 1);

        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());
        ComponentsManager::get()->getComponentSound()->playSound(EngineBuffers::getInstance()->soundPackage->getByLabel("shoot01"),EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
    }
}

void Weapon::shootSmartProjectile(Object3D *parent, Vertex3D position, Vertex3D direction) {
    const int ammoAmount = getAmmoAmount();

    if (ammoAmount <= 0) return;

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);

        auto *componentRender = ComponentsManager::get()->getComponentRender();

        Logging::Log("Weapon shootProjectile from " + parent->getLabel(), "ComponentWeapons");

        auto *projectile = new AmmoSmartProjectileBody();
        projectile->setParent(parent);
        projectile->setLabel("projectile_" + componentRender->getUniqueGameObjectLabel());
        projectile->setWeaponType(this);
        projectile->copyFrom(getModelProjectile());
        projectile->setPosition( position );
        projectile->setEnabled(true);
        projectile->setTTL(EngineSetup::get()->PROJECTILE_DEMO_TTL);
        projectile->makeProjectileRigidBody(
                0.1,
                direction,
                (float) getSpeed(),
                getAccuracy(),
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()
        );

        setAmmoAmount(ammoAmount - 1);

        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());
        ComponentsManager::get()->getComponentSound()->playSound(EngineBuffers::getInstance()->soundPackage->getByLabel("shoot01"),EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
    }
}

void Weapon::shootInstant(Vertex3D from, Object3D *to)
{
    const int ammoAmount = getAmmoAmount();

    if (ammoAmount <= 0) return;

    if (to != nullptr) {
        Drawable::drawLightning(from, to->getPosition());
        setAmmoAmount(ammoAmount - 1);
        auto enemy = dynamic_cast<EnemyGhost*>(to);
        if (enemy != nullptr) {
            enemy->takeDamage(getDamage());
        }
    } else {
        auto closestObject= ComponentsManager::get()->getComponentGame()->getClosesObject3DFromPosition(from, true, false);
        if (closestObject == nullptr) {
            return;
        }
        auto enemy = dynamic_cast<EnemyGhost*>(closestObject);
        if (enemy != nullptr) {
            enemy->takeDamage(getDamage());
            Drawable::drawLightning(from, closestObject->getPosition());
        }
    }
}

void Weapon::reload() {
}

const std::string &Weapon::getSoundEmptyLabel() const {
    return soundEmptyLabel;
}

void Weapon::setSoundEmptyLabel(const std::string &label) {
    Weapon::soundEmptyLabel = label;
}


const std::string &Weapon::getSoundFire() const {
    return fireSound;
}

void Weapon::setSoundFire(const std::string &label) {
    Weapon::fireSound = label;
}

const std::string &Weapon::getLabel() const {
    return label;
}

float Weapon::getCadenceTime() const {
    return cadenceTime;
}

void Weapon::setCadenceTime(float cadenceTime) {
    Weapon::cadenceTime = cadenceTime;
    this->counterCadence->setStep(cadenceTime);
    this->counterCadence->setEnabled(true);
}

Image *Weapon::getIcon() const {
    return icon;
}

void Weapon::setIconImage(std::string file) {
    this->icon = new Image(file);
}

int Weapon::getAmmoAmount() const {
    return ammoAmount;
}

void Weapon::setAmmoAmount(int ammoAmount) {
    Weapon::ammoAmount = ammoAmount;
}

void Weapon::addAmount(int addAmount) {
    this->ammoAmount += addAmount;
}

Mesh3DBody *Weapon::getModelProjectile(){
    return modelProjectile;
}

int Weapon::getType() const {
    return type;
}

void Weapon::setType(int type) {
    Weapon::type = type;
}

