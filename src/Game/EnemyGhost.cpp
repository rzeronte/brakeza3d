
#include "../../include/Game/EnemyGhost.h"
#include "../../include/Physics/Projectile3DBody.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Game/AmmoProjectileGhost.h"


EnemyGhost::EnemyGhost() {
}

void EnemyGhost::shoot() {
   Tools::playMixedSound(EngineBuffers::getInstance()->soundPackage->getSoundByLabel("bulletWhisper"),EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
}

void EnemyGhost::onUpdate() {
    Mesh3D::onUpdate();

    Vector3D way(ComponentsManager::get()->getComponentGame()->getPlayer()->getPosition(), getPosition());

    setRotation(M3::getFromVectors(
        EngineSetup::get()->forward,
        way.getComponent().getNormalize()
    ));

    if (getState() == EnemyState::ENEMY_STATE_DIE) {
        remove();
    }
}


void EnemyGhost::integrate() {
    Mesh3DGhost::integrate();
}

void EnemyGhost::resolveCollision(Collisionable *collisionableObject) {
    Mesh3DGhost::resolveCollision(collisionableObject);

    auto *projectile = dynamic_cast<AmmoProjectileGhost*> (collisionableObject);
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

