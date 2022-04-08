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

typedef enum {
    ITEM_WEAPON_PROJECTILE = 1,
    ITEM_WEAPON_INSTANT = 2,
    ITEM_WEAPON_SMART = 3,
    ITEM_HEALTH = 4,
    ITEM_ENERGY = 5
} LevelInfoItemsTypes;


typedef enum {
    BEHAVIOR_PATROL = 1,
    BEHAVIOR_FOLLOW = 2,
    BEHAVIOR_CIRCLE = 3,
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

    std::string music;
    std::string levelName;
    int currentLevelIndex;
    bool levelStartedToPlay;
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
};


#endif //BRAKEDA3D_LEVELLOADER_H
