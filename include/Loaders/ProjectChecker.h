//
// Created by Eduardo on 23/12/2025.
//

#ifndef BRAKEZA3D_PROJECTCHECKER_H
#define BRAKEZA3D_PROJECTCHECKER_H

#include <string>
#include <vector>
#include "../3D/Vertex3D.h"
#include "../Misc/cJSON.h"

struct ProjectCheckerScript
{
    std::string name;
    std::string typesFile;
    std::string codeFile;
};

struct ProjectCheckerScene
{
    std::string path;
    bool exists = false;
};

struct ProjectCheckerSound
{
    int volumeFX = 50;
    int volumeMusic = 50;
};

struct ProjectCheckerResolution
{
    int width = 1920;
    int height = 1080;
};

struct ProjectCheckerStatus
{
    std::string name;
    std::string description;
    Vertex3D gravity;
    std::vector<ProjectCheckerScript> scripts;
    std::vector<ProjectCheckerScene> scenes;
    ProjectCheckerSound sound;
    ProjectCheckerResolution resolution;
};

class ProjectChecker
{
    std::string path;
    ProjectCheckerStatus status;
    bool loaded = false;

    // JSON Helper functions
    static std::string GetStringFromJSON(cJSON* parent, const char* key, const std::string& defaultValue = "");
    static int GetIntFromJSON(cJSON* parent, const char* key, int defaultValue = 0);
    static float GetFloatFromJSON(cJSON* parent, const char* key, float defaultValue = 0.0f);
    static Vertex3D GetVertex3DFromJSON(cJSON* parent, const char* key);

    // Extraction functions
    void ExtractName(cJSON* json);
    void ExtractDescription(cJSON* json);
    void ExtractGravity(cJSON* json);
    void ExtractScripts(cJSON* json);
    void ExtractScenes(cJSON* json);
    void ExtractSound(cJSON* json);
    void ExtractResolution(cJSON* json);

    // Parsing functions
    ProjectCheckerScript ParseScriptFromJSON(cJSON* scriptJSON) const;
    ProjectCheckerScene ParseSceneFromJSON(cJSON* sceneJSON) const;

    // Drawing helpers
    void DrawFileStatus(const std::string& filepath) const;

public:
    explicit ProjectChecker() = default;
    ~ProjectChecker() = default;

    [[nodiscard]] ProjectCheckerStatus getStatus() const        { return status; }
    [[nodiscard]] std::string getFilePath() const               { return path; }
    [[nodiscard]] bool isLoaded() const                         { return loaded; }

    void DrawInformationTable() const;
    void DrawScriptsTable() const;
    void DrawScenesTable() const;
    void DrawWinProjectInfo() const;

    void ExtractProjectInfo(cJSON* json);
    void ResetStatus();

    void LoadProjectInfoFromDisk(const std::string &pathFile);

    void LoadProjectInfoDialog(const std::string &pathFile);
};

#endif //BRAKEZA3D_PROJECTCHECKER_H