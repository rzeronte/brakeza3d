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
    Color blinkPlayer;
    Color blinkEnemy;
    Color crt;
    Color projectilePlayer;
    Color laserPlayer;
    Color laserEnemy;
    Color projectileEnemy;
    Color rayLightPlayer;
    Color explosionEnemyFrom;
    Color explosionEnemyTo;
    Color particlesPlayerFrom;
    Color particlesPlayerTo;

public:
    PaletteColors() :
        first(Color(77, 120, 78)),
        second(Color(110, 161, 113)),
        third(Color(225, 215, 152)),
        fourth(Color(108, 86, 69)),
        five(Color(22, 22, 22)),
        stamina(Color(168, 48, 22)),
        energy(Color(76, 196, 112)),
        blinkPlayer(Color(112, 0, 0)),
        blinkEnemy(Color(112, 0, 0)),
        crt(Color(138, 104, 0)),
        projectilePlayer(Color(241, 197, 61)),
        laserPlayer(Color(0, 144, 0)),
        laserEnemy(Color(144, 0, 0)),
        projectileEnemy(Color(170, 0, 0)),
        rayLightPlayer(Color(228, 204, 62)),
        explosionEnemyFrom(Color(228, 204, 62)),
        explosionEnemyTo(Color(176, 64, 22)),
        particlesPlayerFrom(fourth),
        particlesPlayerTo(Color(255, 255, 255))
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

    [[nodiscard]] const Color &getBlinkEnemy() const {
        return blinkEnemy;
    }

    [[nodiscard]] const Color &getCRT() const {
        return crt;
    }

    [[nodiscard]] const Color &getProjectilePlayer() const {
        return projectilePlayer;
    }

    [[nodiscard]] const Color &getLaserPlayer() const {
        return laserPlayer;
    }

    [[nodiscard]] const Color &getProjectileEnemy() const {
        return projectileEnemy;
    }

    [[nodiscard]] const Color &getRayLightPlayer() const {
        return rayLightPlayer;
    }

    [[nodiscard]] const Color &getExplosionEnemyFrom() const {
        return explosionEnemyFrom;
    }

    [[nodiscard]] const Color &getExplosionEnemyTo() const {
        return explosionEnemyTo;
    }

    [[nodiscard]] const Color &getBlinkPlayer() const {
        return blinkPlayer;
    }

    [[nodiscard]] const Color &getParticlesPlayerFrom() const {
        return particlesPlayerFrom;
    }

    [[nodiscard]] const Color &getParticlesPlayerTo() const {
        return particlesPlayerTo;
    }

    [[nodiscard]] const Color &getLaserEnemy() const {
        return laserEnemy;
    }
};

#endif //BRAKEZA3D_PALETTECOLORS_H
