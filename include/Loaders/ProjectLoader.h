//
// Created by Eduardo on 28/01/2025.
//

#ifndef BRAKEZA3D_PROJECTLOADER_H
#define BRAKEZA3D_PROJECTLOADER_H

#include <string>
#include <vector>
#include "../Misc/FilePaths.h"

class ProjectLoader
{
    // Entrada "scripts" con "onlyWithCliParam" que no se cargó porque ese parámetro no llegó por
    // --set (p. ej. scripts de test): se guarda tal cual para que SaveProject no la pierda.
    struct SkippedProjectScript {
        std::string name;
        std::string codeFile;
        std::string typesFile;
        std::string cliParam;
    };
    static std::vector<SkippedProjectScript> skippedProjectScripts;

    static bool IsCliParamActive(const std::string &param);
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
