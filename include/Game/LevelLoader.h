//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_LEVELLOADER_H
#define BRAKEDA3D_LEVELLOADER_H


#include <vector>
#include "../Misc/Image.h"
#include "../Objects/Object3D.h"
#include "Weapon.h"
#include "../Misc/cJSON.h"
#include "EnemyGhostRespawner.h"
#include "ItemHealthGhost.h"
#include "AsteroidEnemyGhost.h"

typedef enum {
    ITEM_WEAPON_PROJECTILE = 1,
    ITEM_WEAPON_INSTANT = 2,
    ITEM_WEAPON_SMART = 3,
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
    BEHAVIOR_RANDOM = 4
} EnemyBehaviorTypes;

class LevelLoader {
public:
    LevelLoader(std::string filename);
    void load(int levelIndex);
    bool loadNext();
    void addLevel(std::string filename);
    Weapon *parseWeaponJSON(cJSON *weaponJson);

    Counter *countDown;
    Image *tutorialImage;
    bool hasTutorial;

private:
    std::vector<std::string> levels;
    std::vector<EnemyGhostRespawner*> respawners;

    bool hasMusic;
    std::string music;
    std::string levelName;
    int currentLevelIndex;
    bool levelStartedToPlay;
    bool levelFinished;
public:
    int getNumberLevelEnemies() const;

    bool isLevelStartedToPlay() const;

    void setLevelStartedToPlay(bool levelStartedToPlay);

    void setCurrentLevelIndex(int currentLevelIndex);

    int getCurrentLevelIndex() const;

    int size();

    void loadLevelFromJSON(std::string filePath);

    void startCountDown();

    const std::string &getMusic() const;

    void setMusic(const std::string &music);

    const std::string &getLevelName() const;

    void setLevelName(const std::string &levelName);

    Counter *getCountDown() const;

    bool isHasTutorial() const;

    Image *getTutorialImage() const;

    static void makeItemHealthGhost(Vertex3D position);

    static void makeItemEnergyGhost(Vertex3D position);

    void makeItemWeapon(int index, Vertex3D position);

    bool isLevelFinished() const;

    void setLevelFinished(bool levelFinished);

    EnemyGhost *parseEnemyJSON(cJSON *enemyJSON);

    void parseItemJSON(cJSON *itemJSON);

    void parseEmissorJSON(cJSON *emissorJSON);

    void parseBossJSON(cJSON *bossJSON);

    Point2D parsePositionJSON(cJSON *positionJSON);

    Vertex3D getVertex3DFromJSONPosition(cJSON *positionJSON);

    Vertex3D getWorldPositionFromScreenPoint(Point2D fixedPosition);

    void loadPrevious();

    bool isHaveMusic() const;

    void setHasMusic(bool hasMusic);

    AsteroidEnemyGhost*  parseAsteroidJSON(cJSON *asteroidJSON);
};


#endif //BRAKEDA3D_LEVELLOADER_H
