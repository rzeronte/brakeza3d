//
// Created by darkh on 22/01/2026.
//

#ifndef BRAKEZA3D_PROJECT_H
#define BRAKEZA3D_PROJECT_H

#include <string>
#include <cstring>

#include "ProjectChecker.h"

class Project {
public:
    explicit Project(const std::string &filePath)
    :
        filePath(filePath)
    {
        checker.LoadProjectInfoFromDisk(filePath);
        name = checker.getStatus().name;
        description = checker.getStatus().description;

        // Initialize edit buffers
        strncpy(editName, name.c_str(), sizeof(editName) - 1);
        strncpy(editDescription, description.c_str(), sizeof(editDescription) - 1);
    }

    [[nodiscard]] std::string getFilePath() const           { return filePath; }
    [[nodiscard]] ProjectChecker getChecker() const         { return checker; }

    [[nodiscard]] std::string getName() const               { return name; }
    [[nodiscard]] std::string getDescription() const        { return description; }

    void setName(const std::string &value)                  { name = value; }
    void setDescription(const std::string &value)           { description = value; }

    void reloadChecker()                                    { checker.LoadProjectInfoFromDisk(filePath); }

    // Edit buffers for popup
    char editName[256] = "";
    char editDescription[1024] = "";

    void resetEditBuffers() {
        strncpy(editName, name.c_str(), sizeof(editName) - 1);
        strncpy(editDescription, description.c_str(), sizeof(editDescription) - 1);
    }

    void applyEditBuffers() {
        name = editName;
        description = editDescription;
    }

private:
    std::string filePath;
    std::string name;
    std::string description;
    ProjectChecker checker;
};


#endif //BRAKEZA3D_PROJECT_H