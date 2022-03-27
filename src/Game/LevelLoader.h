//
// Created by eduardo on 20/3/22.
//

#ifndef BRAKEDA3D_LEVELLOADER_H
#define BRAKEDA3D_LEVELLOADER_H


#include <vector>
#include "../../include/Misc/Image.h"
#include "../../include/Objects/Object3D.h"

typedef enum {
    ENEMY = 0,
} LevelInfoObjectTypes;

class LevelLoader {
public:
    LevelLoader(std::string filename);
    void load(int levelIndex);
    bool loadNext();
    void addLevel(std::string filename);
private:
    std::vector<Image *> images;
    int currentLevelIndex;
    bool levelStartedToPlay;
    int numberLevelEnemies;
public:
    int getNumberLevelEnemies() const;

    bool isLevelStartedToPlay() const;

    void setLevelStartedToPlay(bool levelStartedToPlay);

    void setCurrentLevelIndex(int currentLevelIndex);

    int getCurrentLevelIndex() const;

    int size();
};


#endif //BRAKEDA3D_LEVELLOADER_H
