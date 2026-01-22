//
// Created by darkh on 22/01/2026.
//

#ifndef BRAKEZA3D_SCENE_H
#define BRAKEZA3D_SCENE_H

#include <string>

class Scene
{
public:
    explicit Scene(const std::string &file_path)
        : filePath(file_path) {
    }

    [[nodiscard]] std::string getFilePath() const {return filePath; }

private:
    std::string filePath;
};


#endif //BRAKEZA3D_SCENE_H