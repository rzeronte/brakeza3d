//
// Created by darkh on 22/01/2026.
//

#ifndef BRAKEZA3D_SCENE_H
#define BRAKEZA3D_SCENE_H

#include <string>
#include <vector>
#include <filesystem>

#include "SceneChecker.h"

class Object3D;

class Scene
{
public:
    explicit Scene(const std::string &file_path)
        : filePath(file_path)
        , name(std::filesystem::path(file_path).stem().string())
    {
    }

    void setActive(bool value);
    void addObject(Object3D *obj);
    void removeObject(Object3D *obj);

    [[nodiscard]] std::string getFilePath() const           { return filePath; }
    [[nodiscard]] std::string getName() const               { return name; }
    [[nodiscard]] bool isActive() const                     { return active; }
    [[nodiscard]] SceneChecker getChecker() const           { return checker; }
    [[nodiscard]] std::vector<Object3D*>& getObjects()      { return objects; }

private:
    SceneChecker checker;
    std::string filePath;
    std::string name;
    bool active = true;
    std::vector<Object3D*> objects;
};

#endif //BRAKEZA3D_SCENE_H
