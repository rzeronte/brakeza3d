#include "AmmoProjectileBodyEmitter.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

AmmoProjectileBodyEmitter::AmmoProjectileBodyEmitter(
    ProjectileBodyEmmitterType type,
    float step,
    bool stop,
    float stopDuration,
    float stopEvery,
    Weapon *weaponType,
    Color c
)
:
    counter(Counter(step)),
    active(false),
    stop(stop),
    weaponType(weaponType),
    counterStopDuration(Counter(stopDuration)),
    counterStopEvery(Counter(stopEvery)),
    type(type),
    color(c)
{
}

bool AmmoProjectileBodyEmitter::isActive() const {
    return active;
}

void AmmoProjectileBodyEmitter::setActive(bool value) {
    AmmoProjectileBodyEmitter::active = value;
}

void AmmoProjectileBodyEmitter::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved() || !isActive() || !isEnabled())  return;

    this->counter.update();

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

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (this->counter.isFinished()) {
        this->addProjectile();
        this->counter.setEnabled(true);
    }
}

Weapon *AmmoProjectileBodyEmitter::getWeapon() const {
    return weaponType;
}

void AmmoProjectileBodyEmitter::addProjectile()
{
    switch(getType()) {
        case ProjectileBodyEmmitterType::UNIQUE_PROJECTILE: {
            launchUniqueProjectile();
            break;
        }
        case ProjectileBodyEmmitterType::CIRCLE_PROJECTILE: {
            launchCircleProjectiles();
            break;
        }
        case LASER_UNIQUE_PROJECTILE:
            launchUniqueLaser();
            break;
        case LASER_CIRCLE_PROJECTILE:
            launchCircleLaser();
            break;
    }
}

bool AmmoProjectileBodyEmitter::isStop() const {
    return stop;
}

AmmoProjectileBodyEmitter::~AmmoProjectileBodyEmitter()
{
    delete weaponType;
}

ProjectileBodyEmmitterType AmmoProjectileBodyEmitter::getType() const {
    return type;
}

void AmmoProjectileBodyEmitter::launchUniqueProjectile()
{
    if (isRemoved()) return;

    if (weaponType == nullptr) return;

    Brakeza3D::get()->addObject3D(new AmmoProjectileBody(
        getPosition(),
        this,
        weaponType,
        M3::getMatrixIdentity(),
        Vertex3D(0.5, 0.5, 0.5),
        this->AxisForward().getNormalize(),
        weaponType->getDamage(),
        (float) weaponType->getSpeed(),
        100,
        color,
        0.001f,
        EngineSetup::collisionGroups::ProjectileEnemy,
        EngineSetup::collisionGroups::Player,
        new ParticleEmitter(
            ParticleEmitterState::DEFAULT,
            nullptr,
            getPosition(),
            10,
            PaletteColors::getExplosionEnemyFrom(),
            PaletteColors::getExplosionEnemyTo(),
            OCParticlesContext::forProjectile()
        )
    ), Brakeza3D::uniqueObjectLabel("emitterProjectile"));
}

void AmmoProjectileBodyEmitter::launchCircleProjectiles()
{
    const int shoots = 8;
    for (int i = 0; i < shoots; i++) {
        setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(0, 360.0f/(float) shoots, 0));
        launchUniqueProjectile();
    }
}

const Color &AmmoProjectileBodyEmitter::getColor() const {
    return color;
}

void AmmoProjectileBodyEmitter::setColor(const Color &color)
{
    AmmoProjectileBodyEmitter::color = color;
}

void AmmoProjectileBodyEmitter::launchUniqueLaser()
{
    if (isRemoved()) return;

    if (weaponType == nullptr) return;

    auto direction = this->AxisForward().getNormalize();

    Brakeza3D::get()->addObject3D(
        new ProjectileRay(
            this,
            position,
            weaponType->getDamage(),
            direction,
            direction.getScaled((float) weaponType->getSpeed()),
            EngineSetup::collisionGroups::ProjectileEnemy,
            EngineSetup::collisionGroups::Player,
            weaponType->getSpeed(),
            color,
            0.001f,
            false
        ),
        Brakeza3D::uniqueObjectLabel("laser")
    );

}

void AmmoProjectileBodyEmitter::launchCircleLaser()
{
    const int shoots = 8;
    for (int i = 0; i < shoots; i++) {
        setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(0, 360.0f/(float) shoots, 0));
        launchUniqueLaser();
    }
}

