
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
}

void EnemyGhost::loadBlinkShader()
{
    blink = new ShaderBlink(this, Color::red());
    blink->setStep(0.05);
    blink->setEnabled(true);
}

void EnemyGhost::onUpdate()
{
    Mesh3D::onUpdate();

    if (!rotationFrameEnabled) {
        rotateToPlayer();
    }

    if (getWeapon() != nullptr) {
        getWeapon()->onUpdate();
    }

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        makeReward();

        ComponentsManager::get()->getComponentGame()->getPlayer()->increaseKills();

        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("enemyExplosion"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        remove();
    }

    if (getState() != EnemyState::ENEMY_STATE_DIE) {
        shoot(ComponentsManager::get()->getComponentGame()->getPlayer());
    }

}

void EnemyGhost::postUpdate()
{
    Object3D::postUpdate();

    if (!isEnabled()) {
        return;
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
    auto fireworks = new ParticleEmissorFireworks(true, 520, 10, 0.01, Color::red(), 6, 10);
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

void EnemyGhost::rotateToPlayer()
{
    setRotation(M3::getFromVectors(
        EngineSetup::get()->forward,
        Vector3D(
                ComponentsManager::get()->getComponentGame()->getPlayer()->getPosition(),
                getPosition()).getComponent().getNormalize()
        ));
}

void EnemyGhost::integrate()
{
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
        //ComponentsManager::get()->getComponentRender()->updateSelectedObject3DInShaders(nullptr);
    }

    Mesh3DGhost::remove();
}

void EnemyGhost::shoot(Object3D *target)
{
    if (getWeapon() == nullptr) return;

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
