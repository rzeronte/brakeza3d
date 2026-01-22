//
// Created by darkh on 22/01/2026.
//

#ifndef BRAKEZA3D_PROJECT_H
#define BRAKEZA3D_PROJECT_H

#include <string>

class Project {
public:
    explicit Project(const std::string &file_path)
        : filePath(file_path) {
    }

    [[nodiscard]] std::string getFilePath() const { return filePath; }

private:
    std::string filePath;

};


#endif //BRAKEZA3D_PROJECT_H