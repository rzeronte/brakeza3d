//
// Created by Eduardo on 28/01/2025.
//

#ifndef BRAKEZA3D_PROJECTLOADER_H
#define BRAKEZA3D_PROJECTLOADER_H

#include <string>
#include "../Misc/FilePaths.h"

class ProjectLoader
{
public:
    ProjectLoader() = default;

    static void LoadProject(const FilePath::ProjectFile& filename);
    static void SaveProject(const FilePath::ProjectFile& filename);
    static void RemoveProjectScenes();
    static void CreateProject(const FilePath::ProjectFile& filename);
    static void RemoveProject(const FilePath::ProjectFile& filename);
    static void RemoveProjectScripts();
    static void CloseCurrentProject();
};

#endif //BRAKEZA3D_PROJECTLOADER_H
