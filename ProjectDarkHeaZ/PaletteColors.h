//
// Created by eduardo on 6/08/23.
//

#ifndef BRAKEZA3D_PALETTECOLORS_H
#define BRAKEZA3D_PALETTECOLORS_H

#include "../include/Misc/Color.h"

class PaletteColors {

public:
    [[nodiscard]] static Color getMenuOptions() { return {77, 120, 78}; }

    [[nodiscard]] static Color getStatisticsText() { return {110, 161, 113}; }

    [[nodiscard]] static Color getPressKeyToContinue() { return {22, 22, 22}; }

    [[nodiscard]] static Color getStamina() { return {168, 48, 22}; }

    [[nodiscard]] static Color getEnergy() { return {76, 196, 112}; }

    [[nodiscard]] static Color getEnemyBlink() { return {112, 0, 0}; }

    [[nodiscard]] static Color getSavageBlink() { return {112, 0, 0}; }

    [[nodiscard]] static Color getCrt() { return {138, 104, 0}; }

    [[nodiscard]] static Color getPlayerProjectile() { return {241, 197, 61}; }

    [[nodiscard]] static Color getPlayerLaser() { return {0, 144, 0}; }

    [[nodiscard]] static Color getEnemyProjectile() { return {170, 0, 0}; }

    [[nodiscard]] static Color getPlayerRay() { return {228, 204, 62}; }

    [[nodiscard]] static Color getExplosionEnemyFrom() { return {255, 255, 0}; }

    [[nodiscard]] static Color getExplosionEnemyTo() { return {255, 0, 255}; }

    [[nodiscard]] static Color getPlayerDamageBlink() { return {112, 0, 0}; }

    [[nodiscard]] static Color getPlayerBenefitBlink() { return {255, 0, 0}; }

    [[nodiscard]] static Color getParticlesPlayerFrom() { return {255, 0, 0}; }

    [[nodiscard]] static Color getParticlesPlayerTo() { return {255, 255, 0}; }

    [[nodiscard]] static Color getEnemyLaser() { return {144, 0, 0}; }

    [[nodiscard]] static Color getPlayerProjectileLight() { return {163, 142, 81}; }

    [[nodiscard]] static Color getPlayerLaserLight() { return {35, 115, 35}; }

    [[nodiscard]] static Color getPlayerRayLight() { return {168, 152, 63}; }

    [[nodiscard]] static Color getEnemyProjectileLight() { return {170, 0, 0}; }

    [[nodiscard]] static Color getEnemyLaserLight() { return {144, 0, 0}; }

    [[nodiscard]] static Color getEnemyRayLight() { return {170, 0, 0}; }

    [[nodiscard]] static Color getParticlesPlayerSatelliteFrom() { return {58, 156, 0}; }

    [[nodiscard]] static Color getParticlesPlayerSatelliteTo() { return {212, 160, 0}; }

    [[nodiscard]] static Color getParticlesPlayerProjectileFrom() { return {58, 156, 0}; }

    [[nodiscard]] static Color getParticlesPlayerProjectileTo() { return {212, 160, 0}; }

    [[nodiscard]] static Color getParticlesEnemyProjectileFrom() { return {156, 58, 0}; }

    [[nodiscard]] static Color getParticlesEnemyProjectileTo() { return {212, 160, 0}; }

    [[nodiscard]] static Color getEnemyLighting() { return {255, 0, 255}; }

    [[nodiscard]] static Color getPlayerLighting() { return {255, 0, 255}; }

    [[nodiscard]] static Color getEnemyMessage() { return {110, 161, 113}; }


};

#endif //BRAKEZA3D_PALETTECOLORS_H
