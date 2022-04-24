
#include "EnemyGhost.h"
#include "../../../include/Physics/Projectile3DBody.h"
#include "../../../include/Brakeza3D.h"
#include "../weapons/AmmoProjectileBody.h"
#include "../items/ItemEnergyGhost.h"
#include "../items/ItemWeaponGhost.h"

EnemyGhost::EnemyGhost() {
    blink = new ShaderBlink();
    blink->setObject(this);
    blink->setStep(0.05);
    blink->setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
    blink->setEnabled(false);
    blink->setColor(Color::red());
    counterDamageBlink = new Counter(1);
    counterDamageBlink->setEnabled(false);
}

void EnemyGhost::onUpdate()
{
    Mesh3D::onUpdate();

    rotateToPlayer();

    getWeapon()->onUpdate();

    if (ComponentsManager::get()->getComponentGame()->getGameState() != EngineSetup::GameState::GAMING) {
        if (getWeapon()->getType() == WeaponTypes::WEAPON_INSTANT) {
            getWeapon()->stopSoundChannel();
        }
        return;
    }

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        ComponentsManager::get()->getComponentGame()->getPlayer()->increaseKills();

        if (getWeapon()->getType() == WeaponTypes::WEAPON_INSTANT) {
            getWeapon()->stopSoundChannel();
        }

        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("enemyExplosion"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        remove();
    }

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        makeReward();
    }

    shoot(ComponentsManager::get()->getComponentGame()->getPlayer());
}

void EnemyGhost::makeReward()
{
    auto fireworks = new ParticleEmissorFireworks(true, 520, 10, 0.01, Color::red(), 6, 50);
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
            healthItem->setRotationFrame(Vertex3D(0, 1, 0));
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
            healthItem->setRotationFrame(Vertex3D(0, 1, 0));
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
            weaponItem->setRotationFrame(Vertex3D(0, 1, 0));
            weaponItem->setStencilBufferEnabled(true);
            weaponItem->setScale(1);
            weaponItem->clone(playerWeapons[randomWeapon]->getModel());
            weaponItem->makeGhostBody(ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), weaponItem, EngineSetup::Weapon, EngineSetup::Player);
            Brakeza3D::get()->addObject3D(weaponItem, weaponItem->getLabel());
            break;
        }
    }
}

void EnemyGhost::rotateToPlayer() {
    setRotation(M3::getFromVectors(
        EngineSetup::get()->forward,
        Vector3D(
                ComponentsManager::get()->getComponentGame()->getPlayer()->getPosition(),
                getPosition()).getComponent().getNormalize()
        ));
}

void EnemyGhost::postUpdate() {
    Object3D::postUpdate();

    if (counterDamageBlink->isEnabled()) {
        counterDamageBlink->update();
        getBlink()->update();
        if (counterDamageBlink->isFinished()) {
            getBlink()->setEnabled(false);
        }
    }
}

void EnemyGhost::integrate() {
    Mesh3DGhost::integrate();
}

void EnemyGhost::resolveCollision(Collisionable *collisionableObject)
{
    Mesh3DGhost::resolveCollision(collisionableObject);

    auto *projectile = dynamic_cast<AmmoProjectileBody*> (collisionableObject);
    if (projectile != nullptr) {
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("enemyDamage"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        getBlink()->setEnabled(true);
        counterDamageBlink->setEnabled(true);

        auto fireworks = new ParticleEmissorFireworks(true, 520, 5, 0.01, Color::yellow(), 3, 10);
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
        ComponentsManager::get()->getComponentRender()->updateSelectedObject3DInShaders(nullptr);
    }

    Mesh3DGhost::remove();
}

void EnemyGhost::shoot(Object3D *target)
{
    Vector3D way(getPosition(), target->getPosition());

    Vertex3D direction = way.getComponent().getNormalize();
    Vertex3D positionProjectile = getPosition() - AxisUp().getScaled(1000);

    switch(weapon->getType()) {
        case WeaponTypes::WEAPON_PROJECTILE: {
            weapon->shootProjectile(
                this,
                positionProjectile,
                direction,
                EngineSetup::collisionGroups::Player,
                getWeapon()->getModelProjectile()->getFlatColor()
            );
            break;
        }
        case WeaponTypes::WEAPON_INSTANT: {
            weapon->shootInstant(
                getPosition(),
                target
            );
            break;
        }
        case WeaponTypes::WEAPON_SMART_PROJECTILE: {
            weapon->shootSmartProjectile(
                this,
                positionProjectile,
                direction,
                EngineSetup::collisionGroups::Player,
                target,
                getWeapon()->getModelProjectile()->getFlatColor()
            );
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
