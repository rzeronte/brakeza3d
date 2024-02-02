//
// Created by edu on 19/01/24.
//

#include "WeaponBomb.h"
#include "../Player.h"
#include "../Items/ItemBombGhost.h"
#include "../../include/Brakeza3D.h"

WeaponBomb::WeaponBomb(const WeaponAttributes &attributes) : Weapon(attributes) {}

void WeaponBomb::shootBomb(Object3D *parent, Vertex3D position)
{
    if (getAmmoAmount() <= 0) return;

    if (isStop() && counterStopDuration.isEnabled()) {
        return;
    }

    if (counterCadence->isFinished()) {
        counterCadence->setEnabled(true);

        Logging::Log("Weapon shootBomb from %s", parent->getLabel().c_str());

        auto *projectile = new ItemBombGhost(5, this->getDamage(), this);
        projectile->setStencilBufferEnabled(true);
        projectile->setParent(parent);
        projectile->clone(getModelProjectile());
        projectile->setPosition(position);
        projectile->setEnableLights(false);
        projectile->setEnabled(true);
        projectile->setRotationFrame(Tools::randomVertex().getScaled(0.25f));
        projectile->setRotationFrameEnabled(true);
        projectile->setFlatTextureColor(false);
        projectile->makeSimpleGhostBody(
            Vertex3D(1, 1, 1),
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::Player,
            EngineSetup::collisionGroups::Enemy
        );

        setAmmoAmount(ammoAmount - 1);

        ComponentsManager::get()->getComponentSound()->sound("projectileBomb", EngineSetup::SoundChannels::SND_GLOBAL, 0);

        ComponentsManager::get()->getComponentGame()->getLevelLoader()->getStats()->increase(WeaponTypes::WEAPON_BOMB);

        Brakeza3D::get()->addObject3D(projectile, Brakeza3D::uniqueObjectLabel("projectile"));

        Tools::makeFadeInSprite(position, ComponentsManager::get()->getComponentGame()->getFadeInSpriteRed()->getAnimation());
        increaseNumberProjectiles();
    }
}

bool WeaponBomb::shoot(WeaponShootAttributes attributes)
{
    shootBomb(parent, parent->getPosition());

    return true;
}