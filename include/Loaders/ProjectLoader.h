//
// Created by Eduardo on 28/01/2025.
//

#ifndef BRAKEZA3D_PROJECTLOADER_H
#define BRAKEZA3D_PROJECTLOADER_H


#include <string>

class ProjectLoader 
{
public:
    ProjectLoader();

    void loadProject(const std::string& filename);

    static void saveProject(const std::string& filename);

    static void removeProjectScripts();

    static void createProject(const std::string &filename);

    static void removeProject(const std::string &filename);
};


#endif //BRAKEZA3D_PROJECTLOADER_H
