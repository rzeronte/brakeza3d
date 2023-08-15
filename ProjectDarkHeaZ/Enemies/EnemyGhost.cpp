
#include "EnemyGhost.h"
#include "../../include/Brakeza3D.h"

EnemyGhost::EnemyGhost() :
    LivingObject(this),
    RotatableToTarget(ComponentsManager::get()->getComponentGame()->getPlayer(), this, 1.0f),
    rayLight(RayLight(
        false,
        this,
        this->AxisUp().getNormalize(),
        this->AxisUp().getScaled(1100),
        150,
        0,
        PaletteColors::getFirst(),
        EngineSetup::collisionGroups::ProjectileEnemy,
        EngineSetup::collisionGroups::Player | EngineSetup::collisionGroups::Enemy
    )),
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
        PaletteColors::getParticlesPlayerFrom(),
        PaletteColors::getParticlesPlayerTo(),
        OCParticlesContext(
            0.0f,
            0.0025f,
            1.5f,
            45.0f,
            0.0f,
            150.0f,
            50.0f,
            255.0f,
            2.0f,
            0.8f,
            0.98f
        )
    );
    particleEmitter->setEnabled(true);

    setStuck(false);
}

void EnemyGhost::onStart()
{
    blink = new ShaderBlink(true, this, 0.05, PaletteColors::getEnemyBlink());
    zombie = new ShaderZombie(true, EngineSetup::get()->IMAGES_FOLDER + "alien.png", this, this->getOpenClRenderer());
}

void EnemyGhost::onUpdate()
{
    Mesh3DAnimatedGhost::onUpdate();

    if (isRemoved()) return;

    //zombie->update();

    counterLight.update();
    if (counterLight.isFinished()) {
        light->setEnabled(false);
        counterLight.setEnabled(false);
    }

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

    updateLight();

    updateEmitterParticles();

    for ( auto dialog: dialogs) {
        const float staminaPercentage = (getStamina() * 100) / getStartStamina();
        if (staminaPercentage < dialog->staminaPercentage && !dialog->isShowed()) {
            ComponentsManager::get()->getComponentSound()->sound("radioBeep", EngineSetup::SoundChannels::SND_GLOBAL, 0);
            dialog->setShowed(true);
            ComponentsManager::get()->getComponentGame()->setGameState(EngineSetup::RADIO_MESSAGE);
            ComponentsManager::get()->getComponentGame()->setCurrentEnemyDialog(dialog);
        }
    }

    if (getBehavior() != nullptr) {
        particleEmitter->drawOnUpdateSecondPass();
    }

}

void EnemyGhost::updateEmitterParticles()
{
    particleEmitter->shaderParticles->setOrigin(
        Transforms::WorldToPoint(
            getPosition() - AxisUp().getScaled(-700),
            ComponentsManager::get()->getComponentCamera()->getCamera()
        )
    );
    particleEmitter->shaderParticles->setDirection(AxisForward());
    particleEmitter->setPosition(getPosition());
}

void EnemyGhost::onDrawHostBuffer()
{
    Mesh3D::onDrawHostBuffer();

    if (isStuck()) {
        Drawable::drawLightning(getPosition() + Tools::randomVertex().getScaled(4), getPosition() + Tools::randomVertex().getScaled(5), PaletteColors::getFirst());
        counterStuck.update();
        if (counterStuck.isFinished()) {
            unstuck();
        }
    }
}

void EnemyGhost::handleDie()
{
    Brakeza3D::get()->addObject3D(new ShockWave(getPosition(), 0.50, 50, 1, true), Brakeza3D::uniqueObjectLabel("shockWave"));

    makeReward();
    Tools::makeExplosion(this, getPosition(), 5, OCParticlesContext::forExplosion(), PaletteColors::getExplosionEnemyFrom(), PaletteColors::getFirst());
    Tools::makeExplosionSprite(getPosition());

    ComponentsManager::get()->getComponentGame()->getPlayer()->increaseKills();

    ComponentsManager::get()->getComponentSound()->sound("enemyExplosion", EngineSetup::SoundChannels::SND_GLOBAL, 0);

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
            healthItem->setLabel("itemHealth");
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
            healthItem->setLabel("itemEnergy");
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
            int randomWeapon = Tools::random(0, 5);
            bool frameBox = false;
            if (randomWeapon == WEAPON_SHIELD || randomWeapon == WEAPON_REFLECTION || randomWeapon == WEAPON_BOMB) {
                frameBox = true;
            }
            auto *weaponItem = new ItemWeaponGhost(playerWeapons[randomWeapon], frameBox);
            weaponItem->setLabel("itemWeapon");
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
    setRotationTarget(getTarget());
    makeRotationToTarget();
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
        if (projectile->getOwner() != this) {
            ComponentsManager::get()->getComponentSound()->sound("enemyDamage", EngineSetup::SoundChannels::SND_GLOBAL, 0);

            blink->setEnabled(true);
            counterDamageBlink.setEnabled(true);

            auto *body = dynamic_cast<AmmoProjectileBody*> (withObject);
            auto *ray = dynamic_cast<ProjectileRay*> (withObject);

            if (body != nullptr) {
                ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_PROJECTILE);
            }

            if (ray != nullptr) {
                ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increaseHit(WEAPON_LASER);
            }

            this->takeDamage((float) projectile->getDamage());
        }
    }

    auto *satellite = dynamic_cast<PlayerSatellite*> (withObject);
    if (satellite != nullptr) {
        blink->setEnabled(true);
        counterDamageBlink.setEnabled(true);

        this->takeDamage(satellite->getDamage() * Brakeza3D::get()->getDeltaTime());
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
            bool shootResult = weapon->shootProjectile(
                this,
                positionProjectile,
                Vertex3D(0, 0, 0),
                direction,
                getRotation(),
                PaletteColors::getEnemyProjectile(),
                2.5f,
                EngineSetup::collisionGroups::ProjectileEnemy,
                EngineSetup::collisionGroups::Player | EngineSetup::collisionGroups::Enemy,
                false,
                false
            );

            if (shootResult) {
                getLight()->setColor(PaletteColors::getEnemyProjectileLight());
                initLight();
            }
            break;
        }
        case WeaponTypes::WEAPON_LASER: {
            bool shootResult = weapon->shootLaserProjectile(
                this,
                getPosition() - AxisUp().getScaled(1000),
                AxisUp().getInverse(),
                0.1f,
                false,
                PaletteColors::getPlayerRayLight(),
                EngineSetup::collisionGroups::ProjectileEnemy,
                EngineSetup::collisionGroups::Player
            );

            if (shootResult) {
                getLight()->setColor(PaletteColors::getEnemyLaserLight());
                initLight();
            }
            break;
        }
        case WeaponTypes::SHOCK: {
            auto player = ComponentsManager::get()->getComponentGame()->getPlayer();

            if (getPosition().distance(player->getPosition()) < 5000) {
                Drawable::drawLightning(getPosition(), target->getPosition(), PaletteColors::getCrt());

                if (player->getState() == PlayerState::GETTING_DAMAGE || player->getState() == PlayerState::DEAD) {
                    break;
                }

                player->makeStuck(4.0);
            }

            break;
        }
        case WeaponTypes::WEAPON_RAYLIGHT: {
            rayLight.setEnabled(true);
            rayLight.updateDirection(
                rayLight.getParent()->AxisDown().getNormalize(),
                rayLight.getParent()->AxisDown().getScaled(1100)
            );
            bool shootResult = weapon->shootRayLight(rayLight, 0.5, PaletteColors::getEnemyLaser());
            if (shootResult) {
                getLight()->setColor(PaletteColors::getEnemyRayLight());
                initLight();
            }
            break;
        }
    }
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

    ComponentsManager::get()->getComponentSound()->sound("electricStuck", EngineSetup::SoundChannels::SND_GLOBAL, 0);
}

void EnemyGhost::unstuck()
{
    setStuck(false);
    counterStuck.setEnabled(false);

    if (getBehavior() != nullptr) {
        this->getBehavior()->setEnabled(true);
    }
}

Object3D *EnemyGhost::getTarget()
{
    auto game = ComponentsManager::get()->getComponentGame();
    auto target = game->getClosestReflection(getPosition());

    if (target != nullptr) {
        return target;
    }

    return game->getPlayer();
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
    Brakeza3D::get()->addObject3D(ray, Brakeza3D::uniqueObjectLabel("fixedRay"));
}

void EnemyGhost::drawOnUpdateSecondPass()
{
    Mesh3D::drawOnUpdateSecondPass();
}

void EnemyGhost::takeDamage(float damageTaken)
{
    this->stamina -= damageTaken;
    if (this->stamina <= 0) {
        blink->setEnabled(false);
        ComponentsManager::get()->getComponentGame()->getPlayer()->increaseCoins(100);
        setState(EnemyState::ENEMY_STATE_DIE);
    }
}

void EnemyGhost::initLight()
{
    counterLight.setEnabled(true);
    light->setEnabled(true);
}

void EnemyGhost::updateLight()
{
    light->setPosition(getPosition() + lightPositionOffset + AxisUp().getScaled(-700));
    light->onUpdate();
}
