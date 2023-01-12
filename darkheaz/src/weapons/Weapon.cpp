
#include "Weapon.h"
#include "../../../include/Brakeza3D.h"
#include "AmmoProjectileBody.h"
#include "AmmoSmartProjectileBody.h"
#include "../items/ItemBombGhost.h"

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
    this->counterCadence->setEnabled(false);
    this->modelProjectile = new Mesh3D();
    this->type = WeaponTypes::WEAPON_PROJECTILE;
    this->stop = false;
    this->stopDuration = 0;
    this->stopEvery = 0;
    this->soundChannel = EngineSetup::SoundChannels::SND_GLOBAL;
    this->icon = nullptr;
}

void Weapon::onUpdate()
{
    counterCadence->update();

    if (isStop()) {
        counterStopEvery.update();
        counterStopDuration.update();

        if (counterStopEvery.isFinished()) {
            counterStopDuration.setEnabled(true);
            counterStopEvery.setEnabled(false);
        }

        if (counterStopDuration.isFinished()) {
            counterStopEvery.setEnabled(true);
            counterStopDuration.setEnabled(false);
        }
    }
}

void Weapon::stopSoundChannel() const {
    ComponentsManager::get()->getComponentSound()->stopChannel(getSoundChannel());
}

void Weapon::setSpeed(float value) {
    this->speed = value;
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

void Weapon::shootProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, M3 rotation, float intensity, int collisionMask, bool sound)
{
    const int ammoAmount = getAmmoAmount();

    if (ammoAmount <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (counterCadence->isFinished()) {
        const float t = cadenceTime + ((1 - intensity) * cadenceTime);
        this->counterCadence->setStep(t);
        this->counterCadence->setEnabled(true);

        setStatus(WeaponStatus::PRESSED);
        auto *componentRender = ComponentsManager::get()->getComponentRender();
        auto *componentGame = ComponentsManager::get()->getComponentGame();

        auto *projectile = new AmmoProjectileBody();
        projectile->setStencilBufferEnabled(true);
        projectile->setParent(parent);
        projectile->setLabel("projectile_" + componentRender->getUniqueGameObjectLabel());
        projectile->setWeaponType(this);
        projectile->setEnableLights(getModelProjectile()->isEnableLights());
        projectile->setFlatTextureColor(getModelProjectile()->isFlatTextureColor());
        projectile->setFlatColor(getModelProjectile()->getFlatColor());

        if (getType() == WEAPON_BOMB) {
            projectile->setRotationFrameEnabled(true);
            projectile->setRotationFrame(Vertex3D(1, 1, 0));
        }

        projectile->clone(getModelProjectile());
        projectile->setPosition(position);
        projectile->setEnabled(true);
        projectile->setTTL(EngineSetup::get()->PROJECTILE_DEMO_TTL);
        projectile->makeProjectileRigidBody(
            0.1,
            direction,
            rotation,
            (float) getSpeed(),
            getAccuracy(),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Projectile,
            collisionMask
        );

        setAmmoAmount(ammoAmount - 1);

        if (sound) {
            ComponentsManager::get()->getComponentSound()->playSound(
                EngineBuffers::getInstance()->soundPackage->getByLabel("projectileTypeOne"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }

        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());

        Brakeza3D::get()->addObject3D(
            new ParticleEmissorFireworks(position, Vertex3D(0, 4, 5), true, 520, 3, 0.02, getModelProjectile()->getFlatColor(), 3, 2),
            "fireworks" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel()
        );
    }
}

void Weapon::shootSmartProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, M3 rotation, float intensity, int collisionMask, Object3D *target, bool sound)
{
    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (counterCadence->isFinished()) {
        const float t = cadenceTime + ((1 - intensity) * cadenceTime);
        this->counterCadence->setStep(t);
        this->counterCadence->setEnabled(true);

        setStatus(WeaponStatus::PRESSED);

        auto *componentRender = ComponentsManager::get()->getComponentRender();

        Logging::Log("Weapon shootProjectile from " + parent->getLabel(), "ComponentWeapons");

        auto *projectile = new AmmoSmartProjectileBody();
        projectile->setTarget(ComponentsManager::get()->getComponentRender()->getSelectedObject());
        projectile->setStencilBufferEnabled(true);
        projectile->setParent(parent);
        projectile->setLabel("projectile_" + componentRender->getUniqueGameObjectLabel());
        projectile->setWeaponType(this);

        projectile->setEnableLights(getModelProjectile()->isEnableLights());
        projectile->setFlatTextureColor(getModelProjectile()->isFlatTextureColor());
        projectile->setFlatColor(getModelProjectile()->getFlatColor());

        projectile->clone(getModelProjectile());
        projectile->setPosition(position);
        projectile->setEnabled(true);
        projectile->setTTL(EngineSetup::get()->PROJECTILE_DEMO_TTL);

        if (target != nullptr) {
            projectile->setTarget(target);
        }

        projectile->makeProjectileRigidBody(
            0.1,
            direction,
            rotation,
            (float) getSpeed(),
            getAccuracy(),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Projectile,
            collisionMask
        );

        setAmmoAmount(ammoAmount - 1);

        if (sound) {
            ComponentsManager::get()->getComponentSound()->playSound(
                    EngineBuffers::getInstance()->soundPackage->getByLabel("projectileTypeTwo"),
                    EngineSetup::SoundChannels::SND_GLOBAL,
                    0
            );
        }

        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());

        Brakeza3D::get()->addObject3D(
            new ParticleEmissorFireworks(position, Vertex3D(0, 4, 5), true, 520, 3, 0.03, getModelProjectile()->getFlatColor(), 3, 2),
            "fireworks" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel()
        );

    }
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

Mesh3D *Weapon::getModelProjectile(){
    return modelProjectile;
}

int Weapon::getType() const {
    return type;
}

void Weapon::setType(int value) {
    Weapon::type = value;
}

int Weapon::getStatus() const {
    return status;
}

void Weapon::setStatus(int value)
{
    Weapon::status = value;
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
    this->counterStopDuration.setStep(stopDuration);
    this->counterStopDuration.setEnabled(false);
}

float Weapon::getStopEvery() const {
    return stopEvery;
}

void Weapon::setStopEvery(float stopEverySeconds) {
    Weapon::stopEvery = stopEverySeconds;
    this->counterStopEvery.setStep(stopEverySeconds);
    this->counterStopEvery.setEnabled(true);
}

int Weapon::getSoundChannel() const {
    return soundChannel;
}

void Weapon::setSoundChannel(int soundChannel) {
    Weapon::soundChannel = soundChannel;
}

Weapon::~Weapon()
{
    delete modelProjectile;
    delete model;
    delete icon;
    delete counterCadence;
}

void Weapon::shootBomb(Object3D *parent, Vertex3D position)
{
    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);
        setStatus(WeaponStatus::PRESSED);

        auto *componentRender = ComponentsManager::get()->getComponentRender();

        Logging::Log("Weapon shootProjectile from " + parent->getLabel(), "ComponentWeapons");

        auto *projectile = new ItemBombGhost(5);
        projectile->setStencilBufferEnabled(true);
        projectile->setParent(parent);
        projectile->setLabel("projectile_" + componentRender->getUniqueGameObjectLabel());
        projectile->clone(getModelProjectile());
        projectile->setPosition(position);
        projectile->setEnableLights(false);
        projectile->setEnabled(true);
        projectile->setRotationFrame(Tools::randomVertex());
        projectile->setRotationFrameEnabled(true);
        projectile->setFlatTextureColor(false);
        projectile->setDamage(this->getDamage());
        projectile->makeSimpleGhostBody(
            Vertex3D(400, 400, 00),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Player,
            EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::Projectile
        );

        setAmmoAmount(ammoAmount - 1);

        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("projectileTypeThree"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );

        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());
    }
}

void Weapon::shootLaser(ShaderLaser *shaderLaser, float intensity)
{
    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    setAmmoAmount(ammoAmount - 1);

    shaderLaser->setDamage(getDamage());
    shaderLaser->setEnabled(true);
    shaderLaser->setIntensity(intensity);

    if (getStatus() == PRESSED) {
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("laser"),
            EngineSetup::SND_LASER,
            -1
        );
    }

    shaderLaser->update();
}
