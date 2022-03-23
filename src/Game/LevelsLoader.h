//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_LEVELSLOADER_H
#define BRAKEDA3D_LEVELSLOADER_H


#include <vector>
#include "../../include/Misc/Image.h"
#include "../../include/Objects/Object3D.h"

typedef enum {
    ENEMY = 0,
} LevelInfoObjectTypes;

class LevelsLoader {
public:
    LevelsLoader(std::string filename);
    void load(int levelIndex);
    bool loadNext();
    void addLevel(std::string filename);
private:
    std::vector<Image *> images;
    std::vector<Object3D *> objects;
    int currentLevelIndex;
    bool levelStartedToPlay;
    int numberLevelEnemies;
public:
    int getNumberLevelEnemies() const;

    bool isLevelStartedToPlay() const;

    void setLevelStartedToPlay(bool levelStartedToPlay);

    void setCurrentLevelIndex(int currentLevelIndex);

    int getNumberLevel();
};


#endif //BRAKEDA3D_LEVELSLOADER_H
