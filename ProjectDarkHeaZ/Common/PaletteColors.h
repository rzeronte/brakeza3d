//
// Created by eduardo on 6/08/23.
//

#ifndef BRAKEZA3D_PALETTECOLORS_H
#define BRAKEZA3D_PALETTECOLORS_H

#include "../../include/Misc/Color.h"

class PaletteColors {

public:
    [[nodiscard]] static Color getMenuOptions() { return {0.30f, 0.47f, 0.31f}; }

    [[nodiscard]] static Color getStatisticsText() { return {1.0f, 0.f, 0.f}; }

    [[nodiscard]] static Color getPressKeyToContinue() { return {0.09f, 0.09f, 0.09f}; }

    [[nodiscard]] static Color getStamina() { return {0.66f, 0.19f, 0.09f}; }

    [[nodiscard]] static Color getEnergy() { return {0.30f, 0.77f, 0.44f}; }

    [[nodiscard]] static Color getEnemyBlink() { return {0.44f, 0.0f, 0.0f}; }

    [[nodiscard]] static Color getSavageBlink() { return {0.44f, 0.0f, 0.0f}; }

    [[nodiscard]] static Color getCrt() { return {0.1, 0.7, 0}; }

    [[nodiscard]] static Color getPlayerProjectile() { return {0.95f, 0.77f, 0.24f}; }

    [[nodiscard]] static Color getPlayerLaser() { return {0.0f, 0.56f, 0.0f}; }

    [[nodiscard]] static Color getEnemyProjectile() { return {0.67f, 0.0f, 0.0f}; }

    [[nodiscard]] static Color getPlayerRay() { return {0.89f, 0.80f, 0.24f}; }

    [[nodiscard]] static Color getExplosionEnemyFrom() { return {1.0f, 1.0f, 0.0f}; }

    [[nodiscard]] static Color getExplosionEnemyTo() { return {1.0f, 0.0f, 1.0f}; }

    [[nodiscard]] static Color getPlayerDamageBlink() { return {0.44f, 0.0f, 0.0f}; }

    [[nodiscard]] static Color getPlayerBenefitBlink() { return {1.0f, 0.0f, 0.0f}; }

    [[nodiscard]] static Color getParticlesPlayerFrom() { return {1.0f, 1.0f, 1.0f}; }

    [[nodiscard]] static Color getParticlesPlayerTo() { return {1.0f, 1.0f, 0.0f}; }

    [[nodiscard]] static Color getEnemyLaser() { return {0.56f, 0.0f, 0.0f}; }

    [[nodiscard]] static Color getPlayerProjectileLight() { return {0.64f, 0.56f, 0.32f}; }

    [[nodiscard]] static Color getPlayerLaserLight() { return {0.14f, 0.45f, 0.14f}; }

    [[nodiscard]] static Color getPlayerRayLight() { return {0.66f, 0.60f, 0.25f}; }

    [[nodiscard]] static Color getEnemyProjectileLight() { return {0.67f, 0.0f, 0.0f}; }

    [[nodiscard]] static Color getEnemyLaserLight() { return {0.56f, 0.0f, 0.0f}; }

    [[nodiscard]] static Color getEnemyRayLight() { return {0.67f, 0.0f, 0.0f}; }

    [[nodiscard]] static Color getParticlesPlayerSatelliteFrom() { return {0.23f, 0.61f, 0.0f}; }

    [[nodiscard]] static Color getParticlesPlayerSatelliteTo() { return {0.83f, 0.63f, 0.0f}; }

    [[nodiscard]] static Color getParticlesPlayerProjectileFrom() { return {0.23f, 0.61f, 0.0f}; }

    [[nodiscard]] static Color getParticlesPlayerProjectileTo() { return {0.83f, 0.63f, 0.0f}; }

    [[nodiscard]] static Color getParticlesEnemyProjectileFrom() { return {0.61f, 0.23f, 0.0f}; }

    [[nodiscard]] static Color getParticlesEnemyProjectileTo() { return {0.83f, 0.63f, 0.0f}; }

    [[nodiscard]] static Color getEnemyLighting() { return {1.0f, 0.0f, 1.0f}; }

    [[nodiscard]] static Color getPlayerLighting() { return {1.0f, 0.0f, 1.0f}; }

    [[nodiscard]] static Color getEnemyMessage() { return {0.43f, 0.63f, 0.44f}; }

};

#endif //BRAKEZA3D_PALETTECOLORS_H
