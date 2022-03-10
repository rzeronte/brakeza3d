
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

