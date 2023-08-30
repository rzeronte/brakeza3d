//
// Created by eduardo on 26/08/23.
//

#include "BoidEnemyGhost.h"
#include "../PaletteColors.h"

void BoidEnemyGhost::shoot(Object3D *target)
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
                EngineSetup::collisionGroups::Player,
                false,
                false,
                PaletteColors::getParticlesEnemyProjectileFrom(),
                PaletteColors::getParticlesEnemyProjectileTo()
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
    }
}
