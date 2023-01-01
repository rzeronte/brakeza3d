
#include "EnemyGhost.h"
#include "../../../include/Physics/Projectile3DBody.h"
#include "../../../include/Brakeza3D.h"
#include "../weapons/AmmoProjectileBody.h"
#include "../items/ItemEnergyGhost.h"
#include "../items/ItemWeaponGhost.h"

EnemyGhost::EnemyGhost()
{
    counterDamageBlink = new Counter(1);
    counterDamageBlink->setEnabled(false);

    counterStucked = new Counter(5);
    counterStucked->setEnabled(false);

    projectileEmissor = nullptr;

    setStucked(false);
}

void EnemyGhost::loadBlinkShader()
{
    blink = new ShaderBlink(this, Color::red());
    blink->setStep(0.05);
    blink->setEnabled(true);
}

void EnemyGhost::onUpdate()
{
    Mesh3DAnimatedGhost::onUpdate();

    auto playerState = ComponentsManager::get()->getComponentGame()->getPlayer()->getState();

    if (!rotationFrameEnabled) {
        rotateToTarget();
    }

    if (projectileEmissor != nullptr) {
        projectileEmissor->setPosition(getPosition());
        projectileEmissor->onUpdate();
    }

    if (getWeapon() != nullptr) {
        getWeapon()->onUpdate();
    }

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        makeReward();
        makeExplosion();

        ComponentsManager::get()->getComponentGame()->getPlayer()->increaseKills();

        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("enemyExplosion"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        unstuck();
        remove();
    }

    if (isStucked()) {

        Drawable::drawLightning(getPosition() + Tools::randomVertex().getScaled(5), getPosition() + Tools::randomVertex().getScaled(5), Color::cyan());

        counterStucked->update();

        if (counterStucked->isFinished()) {
            unstuck();
        }
    }

    if (getState() != EnemyState::ENEMY_STATE_DIE && (playerState == PlayerState::LIVE || playerState == PlayerState::GETTING_DAMAGE)) {
        shoot(getTarget());
    }
}

void EnemyGhost::postUpdate()
{
    Object3D::postUpdate();

    if (!isEnabled()) {
        return;
    }

    if (projectileEmissor != nullptr) {
        projectileEmissor->postUpdate();
    }

    if (counterDamageBlink->isEnabled()) {
        counterDamageBlink->update();
        getBlink()->update();
        if (counterDamageBlink->isFinished()) {
            getBlink()->setEnabled(false);
            counterDamageBlink->setEnabled(false);
        }
    }
}

void EnemyGhost::makeReward()
{
    if (!isRewards()) return;

    auto fireworks = new ParticleEmissorFireworks(true, 520, 10, 0.01, Color::red(), 6, 15);
    fireworks->setPosition(getPosition());
    fireworks->setRotationFrame(0, 4, 5);
    Brakeza3D::get()->addObject3D(fireworks, "fireworks" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());

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
            healthItem->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), healthItem, EngineSetup::Health, EngineSetup::Player);
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
            healthItem->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), healthItem, EngineSetup::Health, EngineSetup::Player);
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
            weaponItem->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), weaponItem, EngineSetup::Weapon, EngineSetup::Player);
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

void EnemyGhost::resolveCollision(Collisionable *collisionableObject)
{
    Mesh3DAnimatedGhost::resolveCollision(collisionableObject);

    auto *projectile = dynamic_cast<AmmoProjectileBody*> (collisionableObject);
    if (projectile != nullptr) {
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("enemyDamage"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        getBlink()->setEnabled(true);
        counterDamageBlink->setEnabled(true);

        auto fireworks = new ParticleEmissorFireworks(true, 1000, 1, 0.02, Color::green(), 1, 4);
        fireworks->setPosition(projectile->getPosition());
        fireworks->setRotationFrame(0, 4, 5);
        Brakeza3D::get()->addObject3D(fireworks, ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());

        this->takeDamage(projectile->getWeaponType()->getDamage());
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
                EngineSetup::collisionGroups::Player,
                false
            );
            break;
        }
        case WeaponTypes::WEAPON_SMART_PROJECTILE: {
            weapon->shootSmartProjectile(
                this,
                positionProjectile,
                direction,
                getRotation(),
                EngineSetup::collisionGroups::Player,
                target,
                false
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

                player->stuck(4.0);
            }

            break;
        }
    }
}

ShaderBlink *EnemyGhost::getBlink() const {
    return blink;
}

EnemyGhost::~EnemyGhost()
{
    delete counterDamageBlink;
    delete blink;
}

void EnemyGhost::stuck(float time)
{
    counterStucked->setStep(time);
    setStucked(true);
    counterStucked->setEnabled(true);
    if (getBehavior() != nullptr) {
        this->getBehavior()->setEnabled(false);
    }

    ComponentsManager::get()->getComponentSound()->playSound(
        EngineBuffers::getInstance()->soundPackage->getByLabel("electricStuck"),
        EngineSetup::SoundChannels::SND_GLOBAL,
        0
    );
}

void EnemyGhost::unstuck()
{
    setStucked(false);
    counterStucked->setEnabled(false);

    if (getBehavior() != nullptr) {
        this->getBehavior()->setEnabled(true);
    }
}

void EnemyGhost::makeExplosion()
{
    auto sprite = new Sprite3D();

    Vertex3D origin = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();

    Vector3D direction(origin, getPosition());
    sprite->setPosition(origin + direction.getComponent().getNormalize().getScaled(150));

    sprite->linkTextureAnimation(ComponentsManager::get()->getComponentGame()->explosion);
    sprite->setAnimation(0);
    sprite->setAutoRemoveAfterAnimation(true);

    Brakeza3D::get()->addObject3D(sprite, "enemy_explosion_" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());
}

Object3D *EnemyGhost::getTarget()
{
    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();

    Object3D *target = player;
    if (!player->reflection->isHidden()) {
        target = player->reflection;
    }

    return target;
}

void EnemyGhost::setProjectileEmissor(AmmoProjectileBodyEmissor *emissor) {
    EnemyGhost::projectileEmissor = emissor;
}

AmmoProjectileBodyEmissor *EnemyGhost::getProjectileEmissor() const {
    return projectileEmissor;
}
