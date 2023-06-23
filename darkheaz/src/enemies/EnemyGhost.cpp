
#include "EnemyGhost.h"
#include "../../../include/Brakeza3D.h"
#include "../items/ItemEnergyGhost.h"
#include "../items/ItemWeaponGhost.h"

EnemyGhost::EnemyGhost() :
    blink(nullptr),
    rayLight(RayLight(
        false,
        this,
        1000,
        0,
        ComponentsManager::get()->getComponentGame()->getPrimaryColor(),
        EngineSetup::collisionGroups::ProjectileEnemy,
        EngineSetup::collisionGroups::Player
    )),
    counterDamageBlink(Counter(1)),
    counterStuck(Counter(5)),
    projectileEmitter(nullptr)
{
    counterDamageBlink.setEnabled(false);
    counterStuck.setEnabled(false);

    particleEmitter = new ParticleEmitter(
        ParticleEmitterState::DEFAULT,
        this,
        getPosition(),
        0,
        Color::white(),
        Color::yellow(),
        OCParticlesContext(
            98,
            0.0025f,
            0.50f,
            0.0f,
            50.0f,
            95.0f,
            125.0f,
            200.0f,
            1.25,
            0.1f,
            0.99f
        )
    );
    particleEmitter->setEnabled(true);

    setStuck(false);
}

void EnemyGhost::onStart()
{
    blink = new ShaderBlink(true, this, 0.05, ComponentsManager::get()->getComponentGame()->getPrimaryColor());
    zombie = new ShaderZombie(true, EngineSetup::get()->IMAGES_FOLDER + "alien.png", this, this->getOpenClRenderer());
}

void EnemyGhost::onUpdate()
{
    Mesh3DAnimatedGhost::onUpdate();
    zombie->update();

    if (!rotationFrameEnabled) {
        rotateToTarget();
    }

    if (getWeapon() != nullptr) {
        getWeapon()->onUpdate();
    }

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        handleDie();
    }

    if (counterDamageBlink.isEnabled()) {
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            blink->setEnabled(false);
            counterDamageBlink.setEnabled(false);
        }
    }

    auto playerState = ComponentsManager::get()->getComponentGame()->getPlayer()->getState();
    auto gameState = ComponentsManager::get()->getComponentGame()->getGameState();
    if (
        state != EnemyState::ENEMY_STATE_DIE &&
        (playerState == PlayerState::LIVE || playerState == PlayerState::GETTING_DAMAGE) &&
        gameState == EngineSetup::GAMING
    ) {
        if (projectileEmitter != nullptr) {
            projectileEmitter->setPosition(getPosition());
            projectileEmitter->onUpdate();
        }

        shoot(getTarget());
    }

    updateLasers();

    particleEmitter->onUpdate();

}

void EnemyGhost::onDraw()
{
    Mesh3D::onDraw();


    if (isStuck()) {
        Drawable::drawLightning(getPosition() + Tools::randomVertex().getScaled(5), getPosition() + Tools::randomVertex().getScaled(5), Color::cyan());
        counterStuck.update();
        if (counterStuck.isFinished()) {
            unstuck();
        }
    }

    particleEmitter->drawCall();
    particleEmitter->setStopAdd(true);
}

void EnemyGhost::handleDie()
{
    Brakeza3D::get()->addObject3D(
        new ShockWave(getPosition(), 0.50, 50, 1, true),
        "shockWave_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel()
    );
    makeReward();
    makeExplosion();

    ComponentsManager::get()->getComponentGame()->getPlayer()->increaseKills();

    ComponentSound::playSound(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("enemyExplosion"),
        EngineSetup::SND_GLOBAL,
        0
    );
    unstuck();
    remove();
}

void EnemyGhost::updateLasers()
{
    for (auto ray : fixedLasers) {
        ray->setRay(getRotation() * ray->getDirection());
        ray->setPosition(getPosition());
    }
}

void EnemyGhost::postUpdate()
{
    Mesh3D::postUpdate();

    if (!isEnabled()) {
        return;
    }

    if (projectileEmitter != nullptr) {
        projectileEmitter->postUpdate();
    }

}

void EnemyGhost::makeReward()
{
    if (!isRewards()) return;

    auto playerWeapons = ComponentsManager::get()->getComponentGame()->getPlayer()->getWeapons();

    int typePresent = Tools::random(0, 2);
    switch(typePresent) {
        case 0: {
            auto *healthItem = new ItemHealthGhost();
            healthItem->setLabel("item_health");
            healthItem->setEnableLights(true);
            healthItem->setPosition(getPosition());
            healthItem->setRotationFrameEnabled(true);
            healthItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
            healthItem->setStencilBufferEnabled(true);
            healthItem->setScale(1);
            healthItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "red_pill.fbx"));
            healthItem->makeGhostBody(
                ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
                healthItem,
                EngineSetup::Health,
                EngineSetup::Player
            );
            Brakeza3D::get()->addObject3D(healthItem, healthItem->getLabel());
            break;
        }
        case 1: {
            auto *healthItem = new ItemEnergyGhost();
            healthItem->setLabel("item_energy");
            healthItem->setEnableLights(true);
            healthItem->setPosition(getPosition());
            healthItem->setRotationFrameEnabled(true);
            healthItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
            healthItem->setStencilBufferEnabled(true);
            healthItem->setScale(1);
            healthItem->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "pill.fbx"));
            healthItem->makeGhostBody(
                ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
                healthItem,
                EngineSetup::Health,
                EngineSetup::Player
            );
            Brakeza3D::get()->addObject3D(healthItem, healthItem->getLabel());
            break;
        }
        case 2: {
            int randomWeapon = Tools::random(0, 2);
            auto *weaponItem = new ItemWeaponGhost(playerWeapons[randomWeapon]);
            weaponItem->setLabel("item_weapon");
            weaponItem->setEnableLights(false);
            weaponItem->setPosition(getPosition());
            weaponItem->setRotation(0, 0, 180);
            weaponItem->setRotationFrameEnabled(true);
            weaponItem->setRotationFrame(Tools::randomVertex().getScaled(0.5));
            weaponItem->setStencilBufferEnabled(true);
            weaponItem->setScale(1);
            weaponItem->clone(playerWeapons[randomWeapon]->getModel());
            weaponItem->makeGhostBody(
                ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(),
                weaponItem,
                EngineSetup::Weapon,
                EngineSetup::Player
            );
            Brakeza3D::get()->addObject3D(weaponItem, weaponItem->getLabel());
            break;
        }
    }
}

void EnemyGhost::rotateToTarget()
{
    setRotation(M3::getFromVectors(
        EngineSetup::get()->forward,
        Vector3D(
            getTarget()->getPosition(),
            getPosition()).getComponent().getNormalize()
        )
    );
}

void EnemyGhost::integrate()
{
    Mesh3DAnimatedGhost::integrate();
}

void EnemyGhost::resolveCollision(Collisionable *withObject)
{
    Mesh3DAnimatedGhost::resolveCollision(withObject);

    auto *projectile = dynamic_cast<AmmoProjectile*> (withObject);
    if (projectile != nullptr) {
        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("enemyDamage"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        blink->setEnabled(true);
        counterDamageBlink.setEnabled(true);

        auto *body = dynamic_cast<AmmoProjectileBody*> (withObject);
        auto *ray = dynamic_cast<ProjectileRay*> (withObject);

        if (body != nullptr) {
            ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_PROJECTILE);
        }

        if (ray != nullptr) {
            ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_LASER_PROJECTILE);
        }

        this->takeDamage((float) projectile->getDamage());
    }
}

void EnemyGhost::remove()
{
    if (ComponentsManager::get()->getComponentRender()->getSelectedObject() == this) {
        ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
    }

    Mesh3DAnimatedGhost::remove();
}

void EnemyGhost::shoot(Object3D *target)
{
    if (getWeapon() == nullptr || !getWeapon()->isAvailable()) return;

    Vector3D way(getPosition(), target->getPosition());

    Vertex3D direction = way.getComponent().getNormalize();
    Vertex3D positionProjectile = getPosition() - AxisUp().getScaled(1000);

    switch(weapon->getType()) {
        case WeaponTypes::WEAPON_PROJECTILE: {
            weapon->shootProjectile(
                this,
                positionProjectile,
                direction,
                getRotation(),
                1.0,
                EngineSetup::collisionGroups::ProjectileEnemy,
                EngineSetup::collisionGroups::Player,
                false
            );
            break;
        }
        case WeaponTypes::WEAPON_LASER_PROJECTILE: {
            weapon->shootLaserProjectile(
                this,
                getPosition() - AxisUp().getScaled(1000),
                AxisUp().getInverse(),
                0.3,
                false,
                Color::red(),
                EngineSetup::collisionGroups::ProjectileEnemy,
                EngineSetup::collisionGroups::Player
            );
            break;
        }
        case WeaponTypes::SHOCK: {
            auto player = ComponentsManager::get()->getComponentGame()->getPlayer();

            if (getPosition().distance(player->getPosition()) < 5000) {
                Drawable::drawLightning(getPosition(), target->getPosition(), Color::yellow());

                if (player->getState() == PlayerState::GETTING_DAMAGE || player->getState() == PlayerState::DEAD) {
                    break;
                }

                player->makeStuck(4.0);
            }

            break;
        }
        case WeaponTypes::WEAPON_LASER_RAY: {
            rayLight.setEnabled(true);
            weapon->shootRayLight(rayLight, 0.5);
            break;
        }
    }
}

ShaderBlink *EnemyGhost::getBlink()
{
    return blink;
}

EnemyGhost::~EnemyGhost()
{
    delete blink;

    for (auto ray : fixedLasers) {
        ray->setRemoved(true);
    }
}

void EnemyGhost::stuck(float time)
{
    counterStuck.setStep(time);
    setStuck(true);
    counterStuck.setEnabled(true);
    if (getBehavior() != nullptr) {
        this->getBehavior()->setEnabled(false);
    }

    ComponentSound::playSound(
        ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("electricStuck"),
        EngineSetup::SoundChannels::SND_GLOBAL,
        0
    );
}

void EnemyGhost::unstuck()
{
    setStuck(false);
    counterStuck.setEnabled(false);

    if (getBehavior() != nullptr) {
        this->getBehavior()->setEnabled(true);
    }
}

void EnemyGhost::makeExplosion()
{
    /*auto sprite = new Sprite3D(EngineSetup::get()->BILLBOARD_WIDTH_DEFAULT, EngineSetup::get()->BILLBOARD_HEIGHT_DEFAULT);

    Vertex3D origin = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();
    Vector3D direction(origin, getPosition());
    sprite->setPosition(origin + direction.getComponent().getNormalize().getScaled(350));

    sprite->linkTextureAnimation(ComponentsManager::get()->getComponentGame()->getExplosionSpriteTemplate());
    sprite->setAnimation(0);
    sprite->setAutoRemoveAfterAnimation(true);

    Brakeza3D::get()->addObject3D(sprite, "enemy_explosion_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
    */

    Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion());
}

Object3D *EnemyGhost::getTarget()
{
    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();

    Object3D *target = player;
    if (!player->getReflection()->isHidden()) {
        target = (Object3D *) player->getReflection();
    }

    return target;
}

void EnemyGhost::setProjectileEmitter(AmmoProjectileBodyEmitter *emitter)
{
    EnemyGhost::projectileEmitter = emitter;
}

AmmoProjectileBodyEmitter *EnemyGhost::getProjectileEmitter() const
{
    return projectileEmitter;
}

void EnemyGhost::addFixedLaser(ProjectileRay *ray)
{
    fixedLasers.push_back(ray);
    Brakeza3D::get()->addObject3D(ray, "fixed_ray" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
}

ParticleEmitter *EnemyGhost::getParticleEmitter() const {
    return particleEmitter;
}

void EnemyGhost::drawCall()
{
    Object3D::drawCall();
}
