//
// Created by eduardo on 6/08/23.
//

#ifndef BRAKEZA3D_PALETTECOLORS_H
#define BRAKEZA3D_PALETTECOLORS_H

#include "../include/Misc/Color.h"

class PaletteColors {
    Color first;
    Color second;
    Color third;
    Color fourth;
    Color five;
    Color stamina;
    Color energy;
    Color playerBlink;
    Color enemyBlink;
    Color crt;
    Color playerProjectile;
    Color playerLaser;
    Color enemyLaser;
    Color enemyProjectile;
    Color playerRay;
    Color explosionEnemyFrom;
    Color explosionEnemyTo;
    Color particlesPlayerFrom;
    Color particlesPlayerTo;
    Color playerProjectileLight;
    Color playerLaserLight;
    Color playerRayLight;

    Color enemyProjectileLight;
    Color enemyLaserLight;
    Color enemyRayLight;

public:
    PaletteColors() :
        first(Color(77, 120, 78)),
        second(Color(110, 161, 113)),
        third(Color(225, 215, 152)),
        fourth(Color(108, 86, 69)),
        five(Color(22, 22, 22)),
        stamina(Color(168, 48, 22)),
        energy(Color(76, 196, 112)),
        playerBlink(Color(112, 0, 0)),
        enemyBlink(Color(112, 0, 0)),
        crt(Color(138, 104, 0)),
        playerProjectile(Color(241, 197, 61)),
        playerLaser(Color(0, 144, 0)),
        enemyLaser(Color(144, 0, 0)),
        enemyProjectile(Color(170, 0, 0)),
        playerRay(Color(228, 204, 62)),
        explosionEnemyFrom(Color(228, 204, 62)),
        explosionEnemyTo(Color(176, 64, 22)),
        particlesPlayerFrom(fourth),
        particlesPlayerTo(Color(255, 255, 255)),

        playerProjectileLight(Color(163, 142, 81)),
        playerLaserLight(Color(35, 115, 35)),
        playerRayLight(Color(168, 152, 63)),

        enemyProjectileLight(Color(170, 0, 0)),
        enemyLaserLight(Color(144, 0, 0)),
        enemyRayLight(Color(170, 0, 0))
    {
    }

    [[nodiscard]] const Color &getFirst() const {
        return first;
    }

    [[nodiscard]] const Color &getSecond() const {
        return second;
    }

    [[nodiscard]] const Color &getThird() const {
        return third;
    }

    [[nodiscard]] const Color &getFourth() const {
        return fourth;
    }

    [[nodiscard]] const Color &getFive() const {
        return five;
    }

    [[nodiscard]] const Color &getStamina() const {
        return stamina;
    }

    [[nodiscard]] const Color &getEnergy() const {
        return energy;
    }

    [[nodiscard]] const Color &getEnemyBlink() const {
        return enemyBlink;
    }

    [[nodiscard]] const Color &getCrt() const {
        return crt;
    }

    [[nodiscard]] const Color &getPlayerProjectile() const {
        return playerProjectile;
    }

    [[nodiscard]] const Color &getPlayerLaser() const {
        return playerLaser;
    }

    [[nodiscard]] const Color &getEnemyProjectile() const {
        return enemyProjectile;
    }

    [[nodiscard]] const Color &getPlayerRay() const {
        return playerRay;
    }

    [[nodiscard]] const Color &getExplosionEnemyFrom() const {
        return explosionEnemyFrom;
    }

    [[nodiscard]] const Color &getExplosionEnemyTo() const {
        return explosionEnemyTo;
    }

    [[nodiscard]] const Color &getPlayerBlink() const {
        return playerBlink;
    }

    [[nodiscard]] const Color &getParticlesPlayerFrom() const {
        return particlesPlayerFrom;
    }

    [[nodiscard]] const Color &getParticlesPlayerTo() const {
        return particlesPlayerTo;
    }

    [[nodiscard]] const Color &getEnemyLaser() const {
        return enemyLaser;
    }

    [[nodiscard]] const Color &getPlayerProjectileLight() const {
        return playerProjectileLight;
    }

    [[nodiscard]] const Color &getPlayerLaserLight() const {
        return playerLaserLight;
    }

    [[nodiscard]] const Color &getPlayerRayLight() const {
        return playerRayLight;
    }

    [[nodiscard]] const Color &getEnemyProjectileLight() const {
        return enemyProjectileLight;
    }

    [[nodiscard]] const Color &getEnemyLaserLight() const {
        return enemyLaserLight;
    }

    [[nodiscard]] const Color &getEnemyRayLight() const {
        return enemyRayLight;
    }
};

#endif //BRAKEZA3D_PALETTECOLORS_H
