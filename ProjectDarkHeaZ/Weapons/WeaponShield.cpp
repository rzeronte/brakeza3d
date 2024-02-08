//
// Created by edu on 19/01/24.
//

#include "WeaponShield.h"
#include "../Player.h"
#include "../Items/ItemShieldGhost.h"
#include "../../include/Brakeza3D.h"
#include "../Common/ShockWave.h"

WeaponShield::WeaponShield(const WeaponAttributes &attributes) : last(nullptr), Weapon(attributes) {}

[[maybe_unused]] void WeaponShield::shootShield(Object3D *parent, Vertex3D position)
{
    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (numLiveProjectiles > MAX_SIMULTANEOUS_BOMBS) {
        return;
    }

    if (numLiveProjectiles == 0) {
        last = nullptr;
    }

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);
        auto componentGame = ComponentsManager::get()->getComponentGame();
        float ttl = 6;
        if (componentGame->getStoreManager()->isItemEnabled(EngineSetup::StoreItems::ITEM_LONG_LIVE)) {
            ttl = 8;
        }

        auto *projectile = new ItemShieldGhost(ttl, this->getDamage(), this);
        projectile->setStencilBufferEnabled(true);
        projectile->setParent(parent);
        projectile->clone(getModelProjectile());
        projectile->setPosition(position);
        projectile->setEnableLights(false);
        projectile->setEnabled(true);
        projectile->setRotationFrame(Vertex3D(1, 0, 0));
        projectile->setRotationFrameEnabled(true);
        projectile->setFlatTextureColor(false);
        projectile->makeSimpleGhostBody(
            Vertex3D(1, 1, 1),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Player,
            EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::ProjectileEnemy
        );

        setAmmoAmount(ammoAmount - 1);

        ComponentsManager::get()->getComponentSound()->sound("projectileShield", EngineSetup::SoundChannels::SND_GLOBAL, 0);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(WeaponTypes::WEAPON_SHIELD);

        Brakeza3D::get()->addObject3D(projectile, Brakeza3D::uniqueObjectLabel("projectile"));

        Brakeza3D::get()->addObject3D(new ShockWave(position, 0.50, 1, ShockWaveParams::standard(), true), Brakeza3D::uniqueObjectLabel("shockWave"));

        Tools::makeFadeInSprite(position, ComponentsManager::get()->getComponentGame()->getFadeInSpriteBlue()->getAnimation());
        increaseNumberProjectiles();

        if (last == nullptr) {
            last = projectile;
        } else {
            if (numLiveProjectiles > 1) {
                auto r = new RayGhost(
                    10,
                    Color::green(),
                    last,
                    projectile,
                    EngineSetup::collisionGroups::Player,
                    EngineSetup::collisionGroups::Enemy | EngineSetup::collisionGroups::ProjectileEnemy
                );
                last = projectile;
                Brakeza3D::get()->addObject3D(r, "shieldGhost");
            }
        }
    }
}

void WeaponShield::onUpdate() {
    Weapon::onUpdate();
}

bool WeaponShield::shoot(WeaponShootAttributes attributes)
{
    shootShield(parent, parent->getPosition());

    return false;
}