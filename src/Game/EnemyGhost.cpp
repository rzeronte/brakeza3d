
#include "../../include/Game/EnemyGhost.h"
#include "../../include/Physics/Projectile3DBody.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Game/AmmoProjectileBody.h"

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

void EnemyGhost::onUpdate() {
    Mesh3D::onUpdate();

    Vector3D way(ComponentsManager::get()->getComponentGame()->getPlayer()->getPosition(), getPosition());

    getWeapon()->onUpdate();

    setRotation(M3::getFromVectors(
        EngineSetup::get()->forward,
        way.getComponent().getNormalize()
    ));

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        ComponentsManager::get()->getComponentGame()->getPlayer()->increaseKills();

        if (getWeapon()->getType() == WeaponTypes::WEAPON_INSTANT) {
            ComponentsManager::get()->getComponentSound()->stopChannel(getWeapon()->getSoundChannel());
        }

        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("enemyExplosion"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
        remove();
    }

    weapon->counterCadence->update();
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

void EnemyGhost::resolveCollision(Collisionable *collisionableObject) {
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

        auto fireworks = new ParticleEmissorFireworks(true, 520, 5, 0.01, Color::yellow(), 3, 20);
        fireworks->setPosition(projectile->getPosition());
        fireworks->setRotationFrame(0, 4, 5);
        Brakeza3D::get()->addObject3D(fireworks, ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel());

        this->takeDamage(projectile->getWeaponType()->getDamage());
    }
}

void EnemyGhost::remove() {
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
                Color::red()
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
                target
            );
            break;
        }
    }

}

ShaderBlink *EnemyGhost::getBlink() const {
    return blink;
}

