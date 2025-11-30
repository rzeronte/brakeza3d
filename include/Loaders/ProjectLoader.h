//
// Created by Eduardo on 28/01/2025.
//

#ifndef BRAKEZA3D_PROJECTLOADER_H
#define BRAKEZA3D_PROJECTLOADER_H


#include <string>

class ProjectLoader 
{
public:
    ProjectLoader() = default;
    static void LoadProject(const std::string& filename);
    static void SaveProject(const std::string& filename);
    static void CreateProject(const std::string &filename);
    static void RemoveProject(const std::string &filename);
    static void RemoveProjectScripts();
};


#endif //BRAKEZA3D_PROJECTLOADER_H
