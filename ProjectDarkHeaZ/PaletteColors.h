//
// Created by eduardo on 6/08/23.
//

#ifndef BRAKEZA3D_PALETTECOLORS_H
#define BRAKEZA3D_PALETTECOLORS_H

#include "../include/Misc/Color.h"

class PaletteColors {

public:
    [[nodiscard]] static Color getFirst() { return {77, 120, 78}; }

    [[nodiscard]] static Color getSecond() { return {110, 161, 113}; }

    [[nodiscard]] static Color getThird() { return {225, 215, 152}; }

    [[nodiscard]] static Color getFourth() { return {108, 86, 69}; }

    [[nodiscard]] static Color getFive() { return {22, 22, 22}; }

    [[nodiscard]] static Color getStamina() { return {168, 48, 22}; }

    [[nodiscard]] static Color getEnergy() { return {76, 196, 112}; }

    [[nodiscard]] static Color getEnemyBlink() { return {112, 0, 0}; }

    [[nodiscard]] static Color getCrt() { return {138, 104, 0}; }

    [[nodiscard]] static Color getPlayerProjectile() { return {241, 197, 61}; }

    [[nodiscard]] static Color getPlayerLaser() { return {0, 144, 0}; }

    [[nodiscard]] static Color getEnemyProjectile() { return {170, 0, 0}; }

    [[nodiscard]] static Color getPlayerRay() { return {228, 204, 62}; }

    [[nodiscard]] static Color getExplosionEnemyFrom() { return {228, 204, 62}; }

    [[nodiscard]] static Color getExplosionEnemyTo() { return {176, 64, 22}; }

    [[nodiscard]] static Color getPlayerBlink() { return {112, 0, 0}; }

    [[nodiscard]] static Color getParticlesPlayerFrom() { return {108, 86, 69}; }

    [[nodiscard]] static Color getParticlesPlayerTo() { return {255, 255, 255}; }

    [[nodiscard]] static Color getEnemyLaser() { return {144, 0, 0}; }

    [[nodiscard]] static Color getPlayerProjectileLight() { return {163, 142, 81}; }

    [[nodiscard]] static Color getPlayerLaserLight() { return {35, 115, 35}; }

    [[nodiscard]] static Color getPlayerRayLight() { return {168, 152, 63}; }

    [[nodiscard]] static Color getEnemyProjectileLight() { return {170, 0, 0}; }

    [[nodiscard]] static Color getEnemyLaserLight() { return {144, 0, 0}; }

    [[nodiscard]] static Color getEnemyRayLight() { return {170, 0, 0}; }
};

#endif //BRAKEZA3D_PALETTECOLORS_H
