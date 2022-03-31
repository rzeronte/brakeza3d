//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_LEVELLOADER_H
#define BRAKEDA3D_LEVELLOADER_H


#include <vector>
#include "../../include/Misc/Image.h"
#include "../../include/Objects/Object3D.h"
#include "../../include/Game/Weapon.h"
#include "../../include/Misc/cJSON.h"
#include "../../include/Game/EnemyGhostRespawner.h"

typedef enum {
    ENEMY = 0,
} LevelInfoObjectTypes;


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
private:
    std::vector<std::string> levels;
    std::vector<EnemyGhostRespawner*> respawners;

    int currentLevelIndex;
    bool levelStartedToPlay;
public:
    int getNumberLevelEnemies() const;

    bool isLevelStartedToPlay() const;

    void setLevelStartedToPlay(bool levelStartedToPlay);

    void setCurrentLevelIndex(int currentLevelIndex);

    int getCurrentLevelIndex() const;

    int size();

    void loadEnemiesFromJSON(std::string filePath);

    void startRespawners();
};


#endif //BRAKEDA3D_LEVELLOADER_H
