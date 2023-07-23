//
// Created by eduardo on 3/05/23.
//

#ifndef BRAKEZA3D_LEVELSTATS_H
#define BRAKEZA3D_LEVELSTATS_H


#include "src/enemies/AsteroidEnemyGhost.h"
#include "src/items/ItemHealthGhost.h"
#include "src/enemies/EnemyGhostEmitter.h"
#include "../include/Misc/cJSON.h"
#include "src/weapons/Weapon.h"
#include "../include/Objects/Object3D.h"
#include "../include/Misc/Image.h"
#include <vector>

class LevelStats {

public:
    float seconds = 0;
    int projectiles = 0;
    int projectilesLaser = 0;
    int rayLaser = 0;
    int bombs = 0;
    int projectilesWasHit = 0;
    int projectilesLaserWasHit = 0;
    int rayLaserWasHit = 0;
    int bombsWasHit = 0;
    int coinsGained = 0;

    void increaseHit(int weaponType) {
        switch(weaponType) {
            case WEAPON_PROJECTILE: { projectilesWasHit++; break; }
            case WEAPON_RAYLIGHT: { rayLaserWasHit++; break; }
            case WEAPON_BOMB: { bombsWasHit++; break;}
            case WEAPON_LASER: { projectilesLaserWasHit++; break; }
            default: break;
        }
    }

    void increase(int weaponType) {
        switch(weaponType) {
            case WEAPON_PROJECTILE: { projectiles++; break; }
            case WEAPON_RAYLIGHT: { rayLaser++; break; }
            case WEAPON_BOMB: { bombs++; break; }
            case WEAPON_LASER: { projectilesLaser++; break;}
            default: break;
        }
    }

    [[nodiscard]] std::string stats(int weaponType) const {
        std::string output;

        switch(weaponType) {
            case WEAPON_PROJECTILE: { output += Tools::sprintf("%d/%d", projectilesWasHit, projectiles); break; }
            case WEAPON_RAYLIGHT: { output += Tools::sprintf("%d/%d", rayLaserWasHit, rayLaser); break; }
            case WEAPON_BOMB: { output += Tools::sprintf("%d/%d", projectilesLaserWasHit, projectilesLaser); break; }
            case WEAPON_LASER: { output += Tools::sprintf("%d/%d", bombsWasHit, bombs); break; }
            default: break;
        }

        return output;
    }

    [[nodiscard]] int medalType(int weaponType) const
    {
        float percentage;

        switch(weaponType) {
            case WEAPON_PROJECTILE: { percentage = Tools::percentage(projectilesWasHit, projectiles); break; }
            case WEAPON_RAYLIGHT: { percentage = Tools::percentage(rayLaserWasHit, rayLaser); break; }
            case WEAPON_BOMB: { percentage = Tools::percentage(projectilesLaserWasHit, projectilesLaser); break; }
            case WEAPON_LASER: { percentage = Tools::percentage(bombsWasHit, bombs); break; }
            default: break;
        }

        int medalType = 0;
        if ( percentage < 50) {
            medalType = 0;
        } else if (percentage >= 50 && percentage < 75) {
            medalType = 1;
        } else if (percentage >= 75){
            medalType = 2;
        }

        return medalType;
    }

    [[nodiscard]] std::string accuracyPercentageFormatted(int weaponType) const
    {
        std::string output;

        switch(weaponType) {
            case WEAPON_PROJECTILE: { output += Tools::sprintf("%.2f%%", Tools::percentage(projectilesWasHit, projectiles)); break; }
            case WEAPON_RAYLIGHT: { output += Tools::sprintf("%.2f%%", Tools::percentage(rayLaserWasHit, rayLaser)); break; }
            case WEAPON_BOMB: { output += Tools::sprintf("%.2f%%", Tools::percentage(projectilesLaserWasHit, projectilesLaser)); break; }
            case WEAPON_LASER: { output += Tools::sprintf("%.2f%%", Tools::percentage(bombsWasHit, bombs)); break; }
            default: break;
        }

        return output;
    }

    void reset()
    {
        seconds = 0;
        projectiles = 0;
        projectilesLaser = 0;
        rayLaser = 0;
        bombs = 0;
        projectilesWasHit = 0;
        projectilesLaserWasHit = 0;
        rayLaserWasHit = 0;
        bombsWasHit = 0;
        coinsGained = 0;
    }
};


#endif //BRAKEZA3D_LEVELSTATS_H
