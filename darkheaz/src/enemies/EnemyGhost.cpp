
#include "EnemyGhost.h"
#include "../../../include/Brakeza3D.h"
#include "../items/ItemEnergyGhost.h"
#include "../items/ItemWeaponGhost.h"

EnemyGhost::EnemyGhost() :
    blink(nullptr),
    rayLight(RayLight(
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
    rayLight.setEnabled(false);

    setStuck(false);
}

void EnemyGhost::onStart()
{
    blink = new ShaderBlink(this, 0.05, ComponentsManager::get()->getComponentGame()->getPrimaryColor());
    blink->setEnabled(true);
}

void EnemyGhost::onUpdate()
{
    Mesh3DAnimatedGhost::onUpdate();

    auto playerState = ComponentsManager::get()->getComponentGame()->getPlayer()->getState();
    auto gameState = ComponentsManager::get()->getComponentGame()->getGameState();

    if (!rotationFrameEnabled) {
        rotateToTarget();
    }

    if (getWeapon() != nullptr) {
        getWeapon()->onUpdate();
    }

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        makeReward();
        makeExplosion();

        ComponentsManager::get()->getComponentGame()->getPlayer()->increaseKills();

        ComponentSound::playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("enemyExplosion"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        unstuck();
        remove();
    }

    if (isStuck()) {

        Drawable::drawLightning(getPosition() + Tools::randomVertex().getScaled(5), getPosition() + Tools::randomVertex().getScaled(5), Color::cyan());

        counterStuck.update();

        if (counterStuck.isFinished()) {
            unstuck();
        }
    }

    if (getState() != EnemyState::ENEMY_STATE_DIE && (playerState == PlayerState::LIVE || playerState == PlayerState::GETTING_DAMAGE) && gameState == EngineSetup::GAMING) {
        if (projectileEmitter != nullptr) {
            projectileEmitter->setPosition(getPosition());
            projectileEmitter->onUpdate();
        }

        shoot(getTarget());
    }

    updateLasers();
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
    Object3D::postUpdate();

    if (!isEnabled()) {
        return;
    }

    if (projectileEmitter != nullptr) {
        projectileEmitter->postUpdate();
    }

    if (counterDamageBlink.isEnabled()) {
        counterDamageBlink.update();
        blink->update();
        if (counterDamageBlink.isFinished()) {
            blink->setEnabled(false);
            counterDamageBlink.setEnabled(false);
        }
    }
}

void EnemyGhost::makeReward()
{
    if (!isRewards()) return;

    Brakeza3D::get()->addObject3D(
        new ParticleEmitterFireworks(getPosition(), 5, 520, 10, 0.01, Color::red(), 6, 15),
        "fireworks" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel()
    );

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
            EngineBuffers::getInstance()->soundPackage->getByLabel("enemyDamage"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        blink->setEnabled(true);
        counterDamageBlink.setEnabled(true);

        Brakeza3D::get()->addObject3D(
            new ParticleEmitterFireworks(
                getPosition(),
                5,
                1000,
                1,
                0.02,
                Color::green(),
                1,
                4
            ),
            "enemy_fireworks_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel()
        );

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
        EngineBuffers::getInstance()->soundPackage->getByLabel("electricStuck"),
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
    auto sprite = new Sprite3D(EngineSetup::get()->BILLBOARD_WIDTH_DEFAULT, EngineSetup::get()->BILLBOARD_HEIGHT_DEFAULT);

    Vertex3D origin = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();
    Vector3D direction(origin, getPosition());
    sprite->setPosition(origin + direction.getComponent().getNormalize().getScaled(350));

    sprite->linkTextureAnimation(ComponentsManager::get()->getComponentGame()->getExplosionSpriteTemplate());
    sprite->setAnimation(0);
    sprite->setAutoRemoveAfterAnimation(true);

    Brakeza3D::get()->addObject3D(sprite, "enemy_explosion_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
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
