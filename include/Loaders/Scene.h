//
// Created by darkh on 22/01/2026.
//

#ifndef BRAKEZA3D_SCENE_H
#define BRAKEZA3D_SCENE_H

#include <string>

#include "SceneChecker.h"

class Scene
{
public:
    explicit Scene(const std::string &file_path)
        : filePath(file_path) {
    }

    [[nodiscard]] std::string getFilePath() const { return filePath; }
    [[nodiscard]] SceneChecker getChecker() const { return checker; }

private:
    SceneChecker checker;
    std::string filePath;
};


#endif //BRAKEZA3D_SCENE_H