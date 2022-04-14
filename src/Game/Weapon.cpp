
#include "../../include/Game/Weapon.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Game/AmmoProjectileBody.h"
#include "../../include/Game/AmmoSmartProjectileBody.h"

Weapon::Weapon(const std::string& label) {
    this->status = WeaponStatus::NONE;
    this->damage = 0;
    this->ammoAmount = 0;
    this->startAmmoAmount = 0;
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
    this->stop = false;
    this->stopDuration = 0;
    this->stopEvery = 0;
    this->soundChannel = EngineSetup::SoundChannels::SND_GLOBAL;
}

void Weapon::onUpdate() {
    counterCadence->update();

    if (status == WeaponStatus::PRESSED && getType() == WeaponTypes::WEAPON_INSTANT && isAvailable() && getAmmoAmount() > 0) {
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("voltageLoop"),
            getSoundChannel(),
            -1
        );
        setStatus(WeaponStatus::SUSTAINED);
    }

    if (status == WeaponStatus::RELEASED && getType() == WeaponTypes::WEAPON_INSTANT && isAvailable()) {
        ComponentsManager::get()->getComponentSound()->stopChannel(getSoundChannel());
        setStatus(WeaponStatus::NONE);
    }

    if (isStop()) {
        counterStopEvery->update();
        counterStopDuration->update();

        if (counterStopEvery->isFinished()) {
            if (getType() == WeaponTypes::WEAPON_INSTANT) {
                ComponentsManager::get()->getComponentSound()->stopChannel(getSoundChannel());
            }
            counterStopDuration->setEnabled(true);
            counterStopEvery->setEnabled(false);
        }

        if (counterStopDuration->isFinished()) {
            counterStopEvery->setEnabled(true);
            counterStopDuration->setEnabled(false);
            if (getType() == WeaponTypes::WEAPON_INSTANT) {
                ComponentsManager::get()->getComponentSound()->playSound(
                    EngineBuffers::getInstance()->soundPackage->getByLabel("voltageLoop"),
                    getSoundChannel(),
                    -1
                );
            }
        }
    }
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

void Weapon::shootProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, int collisionMask, Color color)
{
    const int ammoAmount = getAmmoAmount();

    if (ammoAmount <= 0) return;

    if (isStop() && counterStopDuration->isEnabled()) {
        return;
    }

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);

        setStatus(WeaponStatus::PRESSED);
        auto *componentRender = ComponentsManager::get()->getComponentRender();

        Logging::Log("Weapon shootProjectile from " + parent->getLabel(), "ComponentWeapons");

        auto *projectile = new AmmoProjectileBody();
        projectile->setParent(parent);
        projectile->setLabel("projectile_" + componentRender->getUniqueGameObjectLabel());
        projectile->setWeaponType(this);
        projectile->setEnableLights(false);
        getModelProjectile()->setFlatColor(color);
        projectile->copyFrom(getModelProjectile());
        projectile->setPosition( position );
        projectile->setEnabled(true);
        projectile->setTTL(EngineSetup::get()->PROJECTILE_DEMO_TTL);
        projectile->makeProjectileRigidBody(
            0.1,
            direction,
            (float) getSpeed(),
            getAccuracy(),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Projectile,
            collisionMask
        );

        setAmmoAmount(ammoAmount - 1);
        //ComponentsManager::get()->getComponentSound()->playSound(EngineBuffers::getInstance()->soundPackage->getByLabel("shoot01"),EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());
    }
}

void Weapon::shootSmartProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, int collisionMask, Object3D *target, Color color) {

    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration->isEnabled()) {
        return;
    }

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);
        setStatus(WeaponStatus::PRESSED);

        auto *componentRender = ComponentsManager::get()->getComponentRender();

        Logging::Log("Weapon shootProjectile from " + parent->getLabel(), "ComponentWeapons");

        auto *projectile = new AmmoSmartProjectileBody();
        projectile->setParent(parent);
        projectile->setLabel("projectile_" + componentRender->getUniqueGameObjectLabel());
        projectile->setWeaponType(this);
        getModelProjectile()->setFlatColor(color);
        projectile->copyFrom(getModelProjectile());
        projectile->setPosition(position);
        projectile->setEnableLights(false);
        projectile->setEnabled(true);
        projectile->setTTL(EngineSetup::get()->PROJECTILE_DEMO_TTL);

        if (target != nullptr) {
            projectile->setTarget(target);
        }

        projectile->makeProjectileRigidBody(
            0.1,
            direction,
            (float) getSpeed(),
            getAccuracy(),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Projectile,
            collisionMask
        );

        setAmmoAmount(ammoAmount - 1);

        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());
        //ComponentsManager::get()->getComponentSound()->playSound(EngineBuffers::getInstance()->soundPackage->getByLabel("shoot01"),EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
    }
}

void Weapon::shootInstant(Vertex3D from, Object3D *to)
{
    const int ammoAmount = getAmmoAmount();

    if (ammoAmount <= 0) return;

    if (isStop() && counterStopDuration->isEnabled()) {
        return;
    }

    if (to != nullptr) {
        auto color = Color::white();
        setAmmoAmount(ammoAmount - 1);
        auto enemy = dynamic_cast<EnemyGhost*>(to);
        if (enemy != nullptr) {
            enemy->takeDamage(getDamage());
        }
        auto player = dynamic_cast<Player*>(to);
        if (player != nullptr) {
            color = Color::red();
            player->takeDamage(getDamage());
        }
        Drawable::drawLightning(from, to->getPosition(), color);
    } else {
        auto closestObject= ComponentsManager::get()->getComponentGame()->getClosesObject3DFromPosition(from, true, false);
        if (closestObject == nullptr) {
            return;
        }
        auto enemy = dynamic_cast<EnemyGhost*>(closestObject);
        if (enemy != nullptr) {
            enemy->takeDamage(getDamage());
            Drawable::drawLightning(from, closestObject->getPosition(), Color::white());
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

int Weapon::getStatus() const {
    return status;
}

void Weapon::setStatus(int status)
{

    Weapon::status = status;
}

int Weapon::getStartAmmoAmount() const {
    return startAmmoAmount;
}

void Weapon::setStartAmmoAmount(int startAmmoAmount) {
    Weapon::startAmmoAmount = startAmmoAmount;
}

bool Weapon::isStop() const {
    return stop;
}

void Weapon::setStop(bool stop) {
    Weapon::stop = stop;
}

float Weapon::getStopDuration() const {
    return stopDuration;
}

void Weapon::setStopDuration(float stopDuration)
{
    Weapon::stopDuration = stopDuration;
    this->counterStopDuration = new Counter(stopDuration);
    this->counterStopDuration->setEnabled(false);
}

float Weapon::getStopEvery() const {
    return stopEvery;
}

void Weapon::setStopEvery(float stopEverySeconds) {
    Weapon::stopEvery = stopEverySeconds;
    this->counterStopEvery = new Counter(stopEverySeconds);
    this->counterStopEvery->setEnabled(true);
}

int Weapon::getSoundChannel() const {
    return soundChannel;
}

void Weapon::setSoundChannel(int soundChannel) {
    Weapon::soundChannel = soundChannel;
}


