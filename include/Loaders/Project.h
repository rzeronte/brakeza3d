//
// Created by darkh on 22/01/2026.
//

#ifndef BRAKEZA3D_PROJECT_H
#define BRAKEZA3D_PROJECT_H

#include <string>

#include "ProjectChecker.h"

class Project {
public:
    explicit Project(const std::string &filePath)
    :
        filePath(filePath)
    {
        checker.LoadProjectInfoFromDisk(filePath);
    }

    [[nodiscard]] std::string getFilePath() const           { return filePath; }
    [[nodiscard]] ProjectChecker getChecker() const         { return checker; }

private:
    std::string filePath;
    ProjectChecker checker;
};


#endif //BRAKEZA3D_PROJECT_H