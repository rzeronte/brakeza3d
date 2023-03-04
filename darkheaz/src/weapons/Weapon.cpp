
#include "Weapon.h"

#include <utility>
#include "../../../include/Brakeza3D.h"
#include "AmmoProjectileBody.h"
#include "../items/ItemBombGhost.h"


Weapon::Weapon(
    const std::string& name,
    const std::string& weaponModel,
    const std::string& projectileModel,
    const std::string& icon,
    Color projectileColor,
    bool projectileFlatTexture,
    bool projectileEnableLights,
    int ammoAmount,
    int startAmmoAmount,
    float damage,
    int speed,
    int dispersion,
    float accuracy,
    float cadenceTime,
    bool stop,
    float stopEver,
    float stopDuration,
    int type,
    bool available
) {
    setLabel(name);
    setStatus(WeaponStatus::NONE);

    this->model = new Mesh3D();
    this->counterCadence = new Counter(this->cadenceTime);
    this->counterCadence->setEnabled(false);
    this->modelProjectile = new Mesh3D();
    this->soundChannel = EngineSetup::SoundChannels::SND_GLOBAL;

    getModel()->AssimpLoadGeometryFromFile(weaponModel);
    getModelProjectile()->setFlatTextureColor(projectileFlatTexture);
    getModelProjectile()->setEnableLights(projectileEnableLights);
    getModelProjectile()->AssimpLoadGeometryFromFile(projectileModel);
    getModelProjectile()->setLabel("projectile_weapon_template" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    getModelProjectile()->setFlatColor(projectileColor);
    getModelProjectile()->setScale(1);

    setIconImage(icon);

    setAmmoAmount(ammoAmount);
    setStartAmmoAmount(startAmmoAmount);
    setSpeed(speed);
    setDamage(damage);
    setDispersion(dispersion);
    setAccuracy(accuracy);
    setCadenceTime(cadenceTime);
    setType(type);

    setStop(stop);
    setAvailable(available);

    if (stop) {
        setStopDuration(stopDuration);
        setStopEvery(stopEver);
    }
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
    ComponentSound::stopChannel(getSoundChannel());
}

void Weapon::setSpeed(int value) {
    this->speed = value;
}


float Weapon::getDamage() const {
    return this->damage;
}

void Weapon::setDamage(float value) {
    this->damage = value;
}

float Weapon::getAccuracy() const {
    return accuracy;
}

void Weapon::setAccuracy(float value) {
    Weapon::accuracy = value;
}

int Weapon::getDispersion() const {
    return dispersion;
}

void Weapon::setDispersion(int value) {
    Weapon::dispersion = value;
}

int Weapon::getSpeed() const {
    return speed;
}

bool Weapon::isAvailable() const {
    return available;
}

void Weapon::setAvailable(bool value) {
    Weapon::available = value;
}

float Weapon::getDamageRadius() const {
    return damageRadius;
}

void Weapon::setDamageRadius(float value) {
    Weapon::damageRadius = value;
}

Mesh3D *Weapon::getModel() const {
    return model;
}

void Weapon::shootProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, M3 rotation, float intensity, int filterGroup, int filterMask, bool sound)
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

        auto *projectile = new AmmoProjectileBody(
            position,
            parent,
            this,
            rotation,
            Vertex3D(50, 50, 50),
            direction,
            getDamage(),
            (float) getSpeed(),
            getAccuracy(),
            filterGroup,
            filterMask,
            nullptr
        );

        auto *projectileParticleEmissor = new ParticleEmitter(projectile, position, 4, 1000, 1, 0.003, this->getModelProjectile()->getFlatColor());
        projectileParticleEmissor->setRotationFrame(0, 20, 20);

        if (getType() == WEAPON_BOMB) {
            projectile->setRotationFrameEnabled(true);
            projectile->setRotationFrame(Vertex3D(1, 1, 0));
        }

        setAmmoAmount(getAmmoAmount() - 1);

        if (sound) {
            ComponentSound::playSound(
                ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("projectileTypeOne"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }

        Brakeza3D::get()->addObject3D(projectile, "projectile_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
        Brakeza3D::get()->addObject3D(projectileParticleEmissor, "particleEm_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    }
}

void Weapon::shootLaserProjectile(Object3D *parent, Vertex3D position, Vertex3D direction, float intensity, bool sound, Color color, int filterGroup, int filterMask)
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

        auto *projectile = new ProjectileRay(
            getDamage(),
            direction,
            direction.getNormalize().getScaled((float) getSpeed()),
            filterGroup,
            filterMask,
            getSpeed(),
            color,
            false
        );

        projectile->setLabel("projectile_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
        projectile->setParent(parent);
        projectile->setPosition(position);

        setAmmoAmount(getAmmoAmount() - 1);

        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());

        if (sound) {
            ComponentSound::playSound(
                ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("laserShoot"),
                EngineSetup::SoundChannels::SND_GLOBAL,
                0
            );
        }
    }
}

const std::string &Weapon::getLabel() const {
    return label;
}

void Weapon::setCadenceTime(float value) {
    Weapon::cadenceTime = value;
    this->counterCadence->setStep(value);
    this->counterCadence->setEnabled(true);
}

Image *Weapon::getIcon() const {
    return icon;
}

void Weapon::setIconImage(const std::string& file) {
    this->icon = new Image(file);
}

int Weapon::getAmmoAmount() const {
    return ammoAmount;
}

void Weapon::setAmmoAmount(int value) {
    Weapon::ammoAmount = value;
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

void Weapon::setStartAmmoAmount(int value) {
    Weapon::startAmmoAmount = value;
}

bool Weapon::isStop() const {
    return stop;
}

void Weapon::setStop(bool value) {
    Weapon::stop = value;
}

float Weapon::getStopDuration() const {
    return stopDuration;
}

void Weapon::setStopDuration(float value)
{
    Weapon::stopDuration = value;
    this->counterStopDuration.setStep(value);
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

void Weapon::setSoundChannel(int value) {
    Weapon::soundChannel = value;
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

        Logging::Log("Weapon shootProjectile from %s", parent->getLabel().c_str());

        auto *projectile = new ItemBombGhost(5, this->getDamage());
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
        projectile->makeSimpleGhostBody(
            Vertex3D(600, 600, 600),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Player,
            EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::ProjectileEnemy
        );

        setAmmoAmount(ammoAmount - 1);

        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("projectileTypeThree"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );

        Brakeza3D::get()->addObject3D(projectile, projectile->getLabel());
    }
}

void Weapon::shootRayLight(RayLight &rayLight, float intensity)
{
    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    setAmmoAmount(ammoAmount - 1);

    rayLight.setColor(getModelProjectile()->getFlatColor());
    rayLight.setDamage(getDamage());
    rayLight.setEnabled(true);
    rayLight.setIntensity(intensity);

    if (getStatus() == PRESSED) {
        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("laser"),
            EngineSetup::SND_LASER,
            -1
        );
    }

    rayLight.update();
}

void Weapon::setLabel(const std::string &value) {
    Weapon::label = value;
}

