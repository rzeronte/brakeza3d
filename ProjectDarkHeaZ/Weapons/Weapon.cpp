
#include "Weapon.h"

#include <utility>
#include "../../include/Brakeza3D.h"
#include "AmmoProjectileBody.h"
#include "../Items/ItemBombGhost.h"
#include "../Items/ItemShieldGhost.h"
#include "../Common/ShockWave.h"


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
    float speed,
    int dispersion,
    float accuracy,
    float cadenceTime,
    bool stop,
    float stopEver,
    float stopDuration,
    int type,
    bool available,
    bool selectable
)
:
    selectable(selectable)
{
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
    getModelProjectile()->setLabel(Brakeza3D::uniqueObjectLabel("projectile_weapon_template"));
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

void Weapon::setSpeed(float value) {
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

float Weapon::getSpeed() const {
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

bool Weapon::shootProjectile(
    Object3D *parent,
    Vertex3D position,
    Vertex3D offsetPosition,
    Vertex3D direction,
    M3 rotation,
    Color color,
    float intensity,
    int filterGroup,
    int filterMask,
    bool sound,
    bool allowMirror,
    Color particlesFrom,
    Color particlesTo
)
{
    if (getAmmoAmount() <= 0) return false;

    if (isStop() && counterStopDuration.isEnabled() ) {
        return false;
    }

    auto storeManager = ComponentsManager::get()->getComponentGame()->getStoreManager();

    if (counterCadence->isFinished()) {
        float t = cadenceTime;

        if (storeManager->isItemEnabled(EngineSetup::StoreItems::ITEM_FAST_SHOOT_CADENCE)){
            t = t - (t * 0.5f);
        }

        this->counterCadence->setStep(t);
        this->counterCadence->setEnabled(true);

        setStatus(WeaponStatus::PRESSED);

        setAmmoAmount(getAmmoAmount() - 1);

        if (sound) {
            ComponentsManager::get()->getComponentSound()->sound("projectileStandard", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(getType());

        Brakeza3D::get()->addObject3D(new AmmoProjectileBody(
            position - offsetPosition,
            parent,
            this,
            rotation,
            Vertex3D(0.1, 0.1, 0.1),
            direction,
            getDamage(),
            getSpeed(),
            getAccuracy(),
            color,
            intensity,
            filterGroup,
            filterMask,
            new ParticleEmitter(
                ParticleEmitterState::DEFAULT,
                nullptr,
                position,
                0,
                particlesFrom,
                particlesTo,
                OCParticlesContext::forProjectile()
            )
        ), Brakeza3D::uniqueObjectLabel("weaponProjectile"));

        if (storeManager->isItemEnabled(EngineSetup::StoreItems::ITEM_MIRROR_SHOOT) && getType() == WEAPON_PROJECTILE && allowMirror) {

            Brakeza3D::get()->addObject3D(new AmmoProjectileBody(
                position + offsetPosition,
                parent,
                this,
                rotation,
                Vertex3D(0.1, 0.1, 0.1),
                direction.getInverse(),
                getDamage(),
                 getSpeed(),
                getAccuracy(),
                color,
                intensity,
                filterGroup,
                filterMask,
                new ParticleEmitter(
                    ParticleEmitterState::DEFAULT,
                    nullptr,
                    position,
                    0,
                    particlesFrom,
                    particlesTo,
                    OCParticlesContext::forProjectile()
            )
            ), Brakeza3D::uniqueObjectLabel("weaponProjectile"));
        }

        return true;
    }

    return false;
}

bool Weapon::shootLaserProjectile(
    Object3D *parent,
    Vertex3D position,
    Vertex3D direction,
    float intensity,
    bool sound,
    Color color,
    int filterGroup,
    int filterMask
)
{
    if (getAmmoAmount() <= 0) return false;

    if (isStop() && counterStopDuration.isEnabled()) {
        return false;
    }

    if (counterCadence->isFinished()) {
        float t = cadenceTime;

        auto storeManager = ComponentsManager::get()->getComponentGame()->getStoreManager();
        if (storeManager->isItemEnabled(EngineSetup::StoreItems::ITEM_FAST_SHOOT_CADENCE)){
            t = t - (t * 0.25f);
        }

        this->counterCadence->setStep(t);
        this->counterCadence->setEnabled(true);


        setStatus(WeaponStatus::PRESSED);

        auto test  = Vertex3D();
        test.x += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));
        test.y += (float) Tools::random((int)(-100 + accuracy), (int)(100 - accuracy));
        test.z = 0;
        test = test.getScaled(0.005);

        auto *projectile = new ProjectileRay(
            parent,
            position,
            getDamage(),
            direction + test,
            direction.getNormalize().getScaled(getSpeed()),
            filterGroup,
            filterMask,
            getSpeed(),
            color,
            intensity,
            false
        );

        setAmmoAmount(getAmmoAmount() - 1);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(WEAPON_LASER);

        Brakeza3D::get()->addObject3D(projectile, Brakeza3D::uniqueObjectLabel("weaponProjectile"));

        if (sound) {
            ComponentsManager::get()->getComponentSound()->sound("projectileLaser", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }

        return true;
    }

    return false;
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

void Weapon::shootShield(Object3D *parent, Vertex3D position)
{
    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);
        setStatus(WeaponStatus::PRESSED);

        Logging::Log("Weapon shootBomb from %s", parent->getLabel().c_str());

        auto *projectile = new ItemShieldGhost(5, this->getDamage());
        projectile->setStencilBufferEnabled(true);
        projectile->setParent(parent);
        projectile->clone(getModelProjectile());
        projectile->setPosition(position);
        projectile->setEnableLights(false);
        projectile->setEnabled(true);
        projectile->setRotationFrame(Vertex3D(1, 0, 0));
        projectile->setRotationFrameEnabled(true);
        projectile->setFlatTextureColor(false);
        projectile->makeSimpleGhostBody(
            Vertex3D(1, 1, 1),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Player,
            EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::ProjectileEnemy
        );

        setAmmoAmount(ammoAmount - 1);

        ComponentsManager::get()->getComponentSound()->sound("projectileShield", EngineSetup::SoundChannels::SND_GLOBAL, 0);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(getType());

        Brakeza3D::get()->addObject3D(projectile, Brakeza3D::uniqueObjectLabel("projectile"));

        Brakeza3D::get()->addObject3D(new ShockWave(position, 0.50, 1, true), Brakeza3D::uniqueObjectLabel("shockWave"));

        Tools::makeFadeInSprite(position, ComponentsManager::get()->getComponentGame()->getFadeInSpriteBlue()->getAnimation());
    }
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

        Logging::Log("Weapon shootBomb from %s", parent->getLabel().c_str());

        auto *projectile = new ItemBombGhost(5, this->getDamage());
        projectile->setStencilBufferEnabled(true);
        projectile->setParent(parent);
        projectile->clone(getModelProjectile());
        projectile->setPosition(position);
        projectile->setEnableLights(false);
        projectile->setEnabled(true);
        projectile->setRotationFrame(Tools::randomVertex().getScaled(0.25f));
        projectile->setRotationFrameEnabled(true);
        projectile->setFlatTextureColor(false);
        projectile->makeSimpleGhostBody(
            Vertex3D(1, 1, 1),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Player,
            EngineSetup::collisionGroups::Enemy
        );

        setAmmoAmount(ammoAmount - 1);

        ComponentsManager::get()->getComponentSound()->sound("projectileBomb", EngineSetup::SoundChannels::SND_GLOBAL, 0);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(getType());

        Brakeza3D::get()->addObject3D(projectile, Brakeza3D::uniqueObjectLabel("projectile"));

        Tools::makeFadeInSprite(position, ComponentsManager::get()->getComponentGame()->getFadeInSpriteRed()->getAnimation());
    }
}

void Weapon::shootHologram(Object3D *parent, Vertex3D position)
{
    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);
        setStatus(WeaponStatus::PRESSED);

        Logging::Log("Weapon shootHologram from %s", parent->getLabel().c_str());

        auto *reflection = new PlayerReflection(5);
        reflection->setStencilBufferEnabled(true);
        reflection->setParent(parent);
        reflection->setLabel(Brakeza3D::uniqueObjectLabel("playerReflection"));
        reflection->clone(getModelProjectile());
        reflection->setPosition(position);
        reflection->setEnableLights(false);
        reflection->setEnabled(true);
        reflection->setRotationFrame(Tools::randomVertex().getScaled(0.25f));
        reflection->setRotationFrameEnabled(true);
        reflection->setFlatTextureColor(false);

        setAmmoAmount(ammoAmount - 1);

        ComponentsManager::get()->getComponentSound()->sound("gravitationalShield", EngineSetup::SoundChannels::SND_GLOBAL, 0);

        ComponentsManager::get()->getComponentGame()->getSwarm()->addPredator(new SwarmObject(reflection));

        Brakeza3D::get()->addObject3D(reflection, reflection->getLabel());
        Tools::makeFadeInSprite(position, ComponentsManager::get()->getComponentGame()->getFadeInSpriteGreen()->getAnimation());
    }
}

bool Weapon::shootRayLight(RayLight &rayLight, float intensity, Color color)
{
    if (getAmmoAmount() <= 0) return false;

    if (isStop() && counterStopDuration.isEnabled()) {
        rayLight.setReach(0);
        return false;
    }

    setAmmoAmount(ammoAmount - 1);

    rayLight.setColor(color);
    rayLight.setDamage(getDamage());
    rayLight.setEnabled(true);
    rayLight.setIntensity(intensity);

    if (getStatus() == PRESSED) {
        ComponentsManager::get()->getComponentSound()->sound("projectileRaylight", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    }

    ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(getType());

    rayLight.update();

    return true;
}

void Weapon::setLabel(const std::string &value) {
    Weapon::label = value;
}

Counter *Weapon::getCounterCadence() const {
    return counterCadence;
}

bool Weapon::isSelectable() const {
    return selectable;
}

void Weapon::setSelectable(bool selectable) {
    Weapon::selectable = selectable;
}

