
#include "../../include/Game/EnemyGhost.h"
#include "../../include/Physics/Projectile3DBody.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Game/AmmoProjectileBody.h"


EnemyGhost::EnemyGhost() {
}

void EnemyGhost::onUpdate() {
    Mesh3D::onUpdate();

    Vector3D way(ComponentsManager::get()->getComponentGame()->getPlayer()->getPosition(), getPosition());

    setRotation(M3::getFromVectors(
        EngineSetup::get()->forward,
        way.getComponent().getNormalize()
    ));

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        ComponentsManager::get()->getComponentGame()->getPlayer()->increaseKills();
        ComponentsManager::get()->getComponentSound()->playSound(
            EngineBuffers::getInstance()->soundPackage->getByLabel("enemyExplosion"),
            EngineSetup::SoundChannels::SND_ENVIRONMENT,
            1
        );
        remove();
    }

    weaponType->counterCadence->update();
}


void EnemyGhost::integrate() {
    Mesh3DGhost::integrate();
}

void EnemyGhost::resolveCollision(Collisionable *collisionableObject) {
    Mesh3DGhost::resolveCollision(collisionableObject);

    auto *projectile = dynamic_cast<AmmoProjectileBody*> (collisionableObject);
    if (projectile != nullptr) {
        this->takeDamage(projectile->getWeaponType()->getDamage());
    }
}

void EnemyGhost::remove() {
    if (ComponentsManager::get()->getComponentRender()->getSelectedObject() == this) {
        ComponentsManager::get()->getComponentRender()->setSelectedObject(nullptr);
        ComponentsManager::get()->getComponentRender()->updateSelectedObject3DInShaders(nullptr);
    }

    removeCollisionObject();
    setRemoved(true);
}

void EnemyGhost::shoot(Object3D *target)
{
    Vector3D way(getPosition(), target->getPosition());
    Enemy::shoot(
        this,
        way.getComponent().getNormalize(),
        getPosition() - AxisUp().getScaled(1000)
    );
}

