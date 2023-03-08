//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_LEVELLOADER_H
#define BRAKEDA3D_LEVELLOADER_H


#include <vector>
#include "../include/Misc/Image.h"
#include "../include/Objects/Object3D.h"
#include "src/weapons/Weapon.h"
#include "../include/Misc/cJSON.h"
#include "src/enemies/EnemyGhostEmitter.h"
#include "src/items/ItemHealthGhost.h"
#include "src/enemies/AsteroidEnemyGhost.h"

typedef enum {
    ITEM_WEAPON_PROJECTILE = 1,
    ITEM_WEAPON_BOMB = 2,
    ITEM_WEAPON_SMART = 3,
    ITEM_WEAPON_LASER = 6,
    ITEM_HEALTH = 4,
    ITEM_ENERGY = 5
} LevelInfoItemsTypes;

typedef enum {
    BOSS_LEVEL_10 = 1,
    BOSS_LEVEL_20 = 2,
    BOSS_LEVEL_30 = 3,
    BOSS_LEVEL_40 = 4,
    BOSS_LEVEL_50 = 5,
    BOSS_LEVEL_60 = 6,
    BOSS_LEVEL_70 = 7,
    BOSS_LEVEL_80 = 8,
    BOSS_LEVEL_90 = 9,
    BOSS_LEVEL_100 = 10,
} BossesTypes;

typedef enum {
    BEHAVIOR_PATROL = 1,
    BEHAVIOR_FOLLOW = 2,
    BEHAVIOR_CIRCLE = 3,
    BEHAVIOR_RANDOM = 4,
    BEHAVIOR_PATH = 5,
    ROTATE_FRAME = 6
} EnemyBehaviorTypes;


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

    void increaseHit(int weaponType) {
        switch(weaponType) {
            case WeaponTypes::WEAPON_PROJECTILE: { projectilesWasHit++; break; }
            case WeaponTypes::WEAPON_LASER_RAY: { rayLaserWasHit++; break; }
            case WeaponTypes::WEAPON_BOMB: { bombsWasHit++; break;}
            case WeaponTypes::WEAPON_LASER_PROJECTILE: { projectilesLaserWasHit++; break; }
            default: break;
        }
    }

    void increase(int weaponType) {
        switch(weaponType) {
            case WeaponTypes::WEAPON_PROJECTILE: { projectiles++; break; }
            case WeaponTypes::WEAPON_LASER_RAY: { rayLaser++; break; }
            case WeaponTypes::WEAPON_BOMB: { bombs++; break; }
            case WeaponTypes::WEAPON_LASER_PROJECTILE: { projectilesLaser++; break;}
            default: break;
        }
    }

    [[nodiscard]] std::string stats(int weaponType) const {
        std::string output;

        switch(weaponType) {
            case WeaponTypes::WEAPON_PROJECTILE: { output += Tools::sprintf("%d -  %d", projectilesWasHit, projectiles); break; }
            case WeaponTypes::WEAPON_LASER_RAY: { output += Tools::sprintf("%d - %d", rayLaserWasHit, rayLaser); break; }
            case WeaponTypes::WEAPON_BOMB: { output += Tools::sprintf("%d - %d", projectilesLaserWasHit, projectilesLaser); break; }
            case WeaponTypes::WEAPON_LASER_PROJECTILE: {  output += Tools::sprintf("%d - %d", bombsWasHit, bombs); break; }
            default: break;
        }

        return output;
    }

    [[nodiscard]] int medalType(int weaponType) const
    {
        float percentage;

        switch(weaponType) {
            case WeaponTypes::WEAPON_PROJECTILE: { percentage = Tools::percentage(projectilesWasHit, projectiles); break; }
            case WeaponTypes::WEAPON_LASER_RAY: { percentage = Tools::percentage(rayLaserWasHit, rayLaser); break; }
            case WeaponTypes::WEAPON_BOMB: { percentage = Tools::percentage(projectilesLaserWasHit, projectilesLaser); break; }
            case WeaponTypes::WEAPON_LASER_PROJECTILE: {  percentage = Tools::percentage(bombsWasHit, bombs); break; }
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
            case WeaponTypes::WEAPON_PROJECTILE: { output += Tools::sprintf("%.2f%%", Tools::percentage(projectilesWasHit, projectiles)); break; }
            case WeaponTypes::WEAPON_LASER_RAY: { output += Tools::sprintf("%.2f%%", Tools::percentage(rayLaserWasHit, rayLaser)); break; }
            case WeaponTypes::WEAPON_BOMB: { output += Tools::sprintf("%.2f%%", Tools::percentage(projectilesLaserWasHit, projectilesLaser)); break; }
            case WeaponTypes::WEAPON_LASER_PROJECTILE: {  output += Tools::sprintf("%.2f%%", Tools::percentage(bombsWasHit, bombs)); break; }
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
    }

};

#define COUNTDOWN_TO_START 3

class LevelLoader {
public:
    explicit LevelLoader(std::string filename);
    void load(int levelIndex);
    bool loadNext();
    void addLevel(std::string filename);
    Weapon *parseWeaponJSON(cJSON *weaponJson);

    Counter countDown;
    Image *tutorialImage;
    bool hasTutorial;
    bool endLevel;
    LevelStats *stats;

private:
    std::vector<std::string> levels;
    std::vector<EnemyGhostEmitter*> enemiesEmitter;

    bool hasMusic;
    std::string music;
    std::string levelName;
    int currentLevelIndex;
    bool levelStartedToPlay;
    bool levelFinished;
public:

    [[nodiscard]] bool isLevelStartedToPlay() const;

    void setLevelStartedToPlay(bool levelStartedToPlay);

    void setCurrentLevelIndex(int currentLevelIndex);

    [[nodiscard]] int getCurrentLevelIndex() const;

    int size();

    void loadLevelFromJSON(const std::string& filePath);

    void startCountDown();

    [[nodiscard]] const std::string &getMusic() const;

    void setMusic(const std::string &music);

    [[nodiscard]] const std::string &getLevelName() const;

    void setLevelName(const std::string &levelName);

    Counter * getCountDown();

    [[nodiscard]] bool isHasTutorial() const;

    [[nodiscard]] Image * getTutorialImage() const;

    static void makeItemHealthGhost(Vertex3D position);

    static void makeItemEnergyGhost(Vertex3D position);

    void makeItemWeapon(int index, Vertex3D position);

    [[nodiscard]] bool isLevelFinished() const;

    void setLevelFinished(bool levelFinished);

    void parseEnemyJSON(cJSON *enemyJSON, EnemyGhost *enemy);

    void setBehaviorFromJSON(cJSON *behavior, EnemyGhost *enemy);

    void parseItemJSON(cJSON *itemJSON);

    void parseBossJSON(cJSON *bossJSON);

    Point2D parsePositionJSON(cJSON *positionJSON);
    Vertex3D parseVertex3DJSON(cJSON *vertex3DJSON);

    void setProjectileEmitterForEnemy(cJSON *emitter, EnemyGhost *enemy);

    void setLasersForEnemy(cJSON *lasers, EnemyGhost *enemy);
    void addLasersForEnemy(cJSON *laser, EnemyGhost *enemy);

    Vertex3D getVertex3DFromJSONPosition(cJSON *positionJSON);

    Vertex3D getWorldPositionFromScreenPoint(Point2D fixedPosition);

    void loadPrevious();

    [[nodiscard]] bool isHaveMusic() const;

    void setHasMusic(bool hasMusic);

    AsteroidEnemyGhost*  parseAsteroidJSON(cJSON *asteroidJSON);

    Point2D convertPointPercentRelativeToScreen(Point2D point);

    static Color parseColorJSON(cJSON *color);

    [[nodiscard]] const std::vector<std::string> &getLevels() const;

    [[nodiscard]] bool isEndLevel() const;

    void setEndLevel(bool value);

    LevelStats *getStats() const;
};


#endif //BRAKEDA3D_LEVELLOADER_H
