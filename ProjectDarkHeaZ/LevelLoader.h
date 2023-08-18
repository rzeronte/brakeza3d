//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_LEVELLOADER_H
#define BRAKEDA3D_LEVELLOADER_H


#include <vector>
#include "../include/Misc/Image.h"
#include "../include/Objects/Object3D.h"
#include "Weapons/Weapon.h"
#include "../include/Misc/cJSON.h"
#include "Enemies/EnemyGhostEmitter.h"
#include "Items/ItemHealthGhost.h"
#include "Enemies/AsteroidEnemyGhost.h"
#include "LevelStats.h"
#include "Items/ItemEnergyGhost.h"
#include "Items/ItemWeaponGhost.h"
#include "Items/ItemHumanGhost.h"
#include "Items/SalvageSpaceship.h"

typedef enum {
    ITEM_WEAPON_PROJECTILE = 1,
    ITEM_WEAPON_BOMB = 2,
    ITEM_WEAPON_LASER = 3,
    ITEM_WEAPON_RAYLIGHT = 6,
    ITEM_HEALTH = 4,
    ITEM_ENERGY = 5,
    ITEM_HUMAN = 7,
    ITEM_SHIELD = 8,
    ITEM_WEAPON_REFLECTION = 9,
    ITEM_SALVAGE_SPACESHIP = 10

} LevelInfoItemsTypes;

typedef enum {
    BOSS_LEVEL_10 = 1,
    BOSS_LEVEL_20 = 2,
    BOSS_LEVEL_30 = 3,
} BossesTypes;

typedef enum {
    BEHAVIOR_PATROL = 1,
    BEHAVIOR_FOLLOW = 2,
    BEHAVIOR_CIRCLE = 3,
    BEHAVIOR_RANDOM = 4,
    BEHAVIOR_PATH = 5,
    ROTATE_FRAME = 6
} EnemyBehaviorTypes;


class LevelLoader {
public:
    explicit LevelLoader(std::string filename);
    void load(int levelIndex);
    bool loadNext();
    void addLevel(std::string filename);
    Weapon *parseWeaponJSON(cJSON *weaponJson);

    Counter countDown;
    std::vector<Image*> levelHistories;
    std::vector<Image*> levelTutorials;
    int currentTutorialIndex;

    bool endLevel;
    LevelStats *stats;
    std::vector<Object3D*> objectsBackground;
private:
    std::vector<std::string> levels;
    std::vector<EnemyGhostEmitter*> enemiesEmitter;

    bool hasMusic;
    std::string music;
    std::string levelName;
    int currentLevelIndex;
    bool levelStartedToPlay;
    bool levelFinished;

    Counter waitingToWin;
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

    static ItemHealthGhost* makeItemHealthGhost(Vertex3D position);

    static ItemEnergyGhost* makeItemEnergyGhost(Vertex3D position);

    ItemWeaponGhost* makeItemWeapon(int index, Vertex3D position);

    [[nodiscard]] bool isLevelFinished() const;

    void setLevelFinished(bool levelFinished);

    void parseEnemyJSON(cJSON *enemyJSON, EnemyGhost *enemy);

    void setBehaviorFromJSON(cJSON *behavior, Object3D *enemy, float depth);

    void parseItemJSON(cJSON *itemJSON);

    void parseBossJSON(cJSON *bossJSON);

    Point2D parsePositionJSON(cJSON *positionJSON);
    Vertex3D parseVertex3DJSON(cJSON *vertex3DJSON);

    void setProjectileEmitterForEnemy(cJSON *emitter, EnemyGhost *enemy);

    void setLasersForEnemy(cJSON *lasers, EnemyGhost *enemy);
    void addLasersForEnemy(cJSON *laser, EnemyGhost *enemy);

    Vertex3D getVertex3DFromJSONPosition(cJSON *positionJSON, float depth);

    Vertex3D getPositionFromScreenPoint(Point2D fixedPosition, float depth);

    void loadPrevious();

    [[nodiscard]] bool isHaveMusic() const;

    void setHasMusic(bool hasMusic);

    AsteroidEnemyGhost* parseAsteroidJSON(cJSON *asteroidJSON);

    Point2D convertPointPercentRelativeToScreen(Point2D point);

    static Color parseColorJSON(cJSON *color);

    [[nodiscard]] const std::vector<std::string> &getLevels() const;

    [[nodiscard]] bool isEndLevel() const;

    void setEndLevel(bool value);

    [[nodiscard]] LevelStats *getStats() const;

    void parseBackgroundItem(cJSON *object);

    void removeBackgroundObjects();

    void moveBackgroundObjects(Vertex3D offset);

    static void parseMessageJSON(cJSON *pJson, EnemyGhost *pGhost);

    [[nodiscard]] std::vector<Image*> &getTutorials();

    void drawCurrentTutorialImage(float alpha);

    void increaseTutorialImage();

    void decreaseTutorialImage();

    [[nodiscard]] int getCurrentTutorialIndex() const;

    [[nodiscard]] Counter *getWaitingToWin();

    ItemHumanGhost* makeItemHuman(Vertex3D vertex3D);

    SalvageSpaceship* makeSalvageSpaceship(Vertex3D vertex3D);

    void loadConfig();

    void updateConfig(int level, const char *gpu);
};


#endif //BRAKEDA3D_LEVELLOADER_H
