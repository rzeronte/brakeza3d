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
    BOSS_LEVEL_15 = 2,
    BOSS_LEVEL_20 = 3,
    BOSS_LEVEL_25 = 4,
    BOSS_LEVEL_30 = 5,
} BossesTypes;

typedef enum {
    BEHAVIOR_NONE = 0,
    BEHAVIOR_PATROL = 1,
    BEHAVIOR_FOLLOW = 2,
    BEHAVIOR_CIRCLE = 3,
    BEHAVIOR_RANDOM = 4,
    BEHAVIOR_PATH = 5,
    ROTATE_FRAME = 6
} EnemyBehaviorTypes;

static std::map<std::string, EnemyBehaviorTypes> behaviorTypes = {
    {"none", EnemyBehaviorTypes::BEHAVIOR_NONE},
    {"patrol", EnemyBehaviorTypes::BEHAVIOR_PATROL},
    {"follow", EnemyBehaviorTypes::BEHAVIOR_FOLLOW},
    {"circle", EnemyBehaviorTypes::BEHAVIOR_CIRCLE},
    {"random", EnemyBehaviorTypes::BEHAVIOR_RANDOM},
    {"path", EnemyBehaviorTypes::BEHAVIOR_PATH},
    {"rotation", EnemyBehaviorTypes::ROTATE_FRAME},
};

class LevelLoader {
public:
    explicit LevelLoader(std::string filename);
    void load(int levelIndex);
    bool loadNext();
    void addLevel(std::string filename);
    static Weapon *parseWeaponJSON(Object3D *parent, cJSON *weaponJson, bool isPlayer);

    Counter countDown;
    std::vector<Image*> levelHistories;
    std::vector<Image*> levelTutorials;
    int currentTutorialIndex;

    bool endLevel;
    LevelStats *stats;
    EnemyDialog mainMessage;
    float difficultyRatio;
    int indexSpaceshipSelected;
    bool loaded = false;
private:
    std::vector<Weapon *> weapons;

    std::vector<std::string> levels;
    std::vector<EnemyGhostEmitter*> enemiesEmitter;

    bool hasMusic;
    std::string music;
    std::string levelName;
    std::string levelScene;
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

    static ItemWeaponGhost* makeItemWeapon(int index, Vertex3D position);

    [[nodiscard]] bool isLevelFinished() const;

    void setLevelFinished(bool levelFinished);

    void parseEnemyJSON(cJSON *enemyJSON, EnemyGhost *enemy);

    static void setBehaviorFromJSON(cJSON *behavior, Object3D *enemy, float depth);

    static void parseItemJSON(cJSON *itemJSON);

    void parseBossJSON(cJSON *bossJSON);

    static Point2D parsePositionJSON(cJSON *positionJSON);
    static Vertex3D parseVertex3DJSON(cJSON *vertex3DJSON);

    static void addProjectileEmitterToEnemy(cJSON *emitter, EnemyGhost *enemy);

    static void setLasersForEnemy(cJSON *lasers, EnemyGhost *enemy);
    static void addLasersForEnemy(cJSON *laser, EnemyGhost *enemy);

    static Vertex3D getVertex3DFromJSONPosition(cJSON *positionJSON, float depth);

    static Vertex3D getPositionFromScreenPoint(Point2D fixedPosition, float depth);

    void loadPrevious();

    [[nodiscard]] bool isHaveMusic() const;

    void setHasMusic(bool hasMusic);

    static Color parseColorJSON(cJSON *color);

    [[nodiscard]] const std::vector<std::string> &getLevels() const;

    [[nodiscard]] bool isEndLevel() const;

    void setEndLevel(bool value);

    [[nodiscard]] LevelStats *getStats() const;

    static void removeBackgroundObjects();

    static void moveBackgroundObjects(Vertex3D offset);

    static void parseMessageJSON(cJSON *pJson, EnemyGhost *pGhost);

    [[nodiscard]] std::vector<Image*> &getTutorials();

    void drawCurrentTutorialImage(float alpha);

    void increaseTutorialImage();

    void decreaseTutorialImage();

    [[nodiscard]] int getCurrentTutorialIndex() const;

    [[nodiscard]] Counter *getWaitingToWin();

    static ItemHumanGhost* makeItemHuman(Vertex3D vertex3D);

    static SalvageSpaceship* makeSalvageSpaceship(Vertex3D vertex3D);

    void LoadConfig();

    void updateConfig(int level) const;

    void parseMainMessageJSON(cJSON *message);

    [[nodiscard]] EnemyDialog *getMainMessage();

    void setLevelScene(const std::string &levelScene);

    void LoadJSONWeapons();

    [[nodiscard]] const std::vector<Weapon *> &getWeapons() const;

    virtual ~LevelLoader();

    void reload();

    void increaseDifficulty();

    void decreaseDifficulty();

    int getIndexSpaceshipSelected() const;

    bool isLoaded() const;
};


#endif //BRAKEDA3D_LEVELLOADER_H
