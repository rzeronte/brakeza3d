//
// Created by Eduardo on 23/12/2025.
//

#ifndef BRAKEZA3D_SCENECHECKER_H
#define BRAKEZA3D_SCENECHECKER_H

#include <string>

#include "../3D/Object3D.h"
#include "../3D/Vertex3D.h"
#include "../Misc/cJSON.h"
#include "../Render/Image.h"

struct SceneCheckerShader
{
    std::string name;
    std::string vsFile;
    std::string fsFile;
    std::string typesFile;
};

struct SceneCheckerScript
{
    std::string name;
    std::string typesFile;
    std::string codeFile;
};

struct SceneCheckerObjectInfo {
    std::string name;
    ObjectType type;
    std::vector<SceneCheckerScript> scripts;
    std::vector<SceneCheckerShader> shaders;
    bool collider = false;
    bool enabled = false;
};

struct SceneCheckerStatus
{
    Vertex3D gravity;

    Vertex3D direction;
    Vertex3D ambient;
    Vertex3D diffuse;
    Vertex3D specular;

    int renderWidth = 0;
    int renderHeight = 0;

    Vertex3D cameraPosition;
    Vertex3D cameraRotation;

    std::vector<SceneCheckerObjectInfo> objects;
    std::vector<SceneCheckerScript> scripts;
    std::vector<SceneCheckerShader> shaders;
};

class SceneChecker
{
    std::string path;
    SceneCheckerStatus status;
    bool loaded = false;
    Image* screenshot = nullptr;
public:
    explicit SceneChecker() = default;

    void DrawInformationTable() const;

    void DrawScriptsTable() const;

    void DrawObjectScriptsTable(const SceneCheckerObjectInfo &o) const;

    void DrawObjectsTable() const;

    void DrawShadersTable() const;

    void DrawWinSceneInfo() const;
    void ExtractSceneInfo(cJSON* json);
    void ResetStatus();
    void LoadSceneInfoDialog(const std::string &pathFile);

    static std::string getStringObjectType(ObjectType type);
    static GUIType::Sheet getIconObject(ObjectType type);


};


#endif //BRAKEZA3D_SCENECHECKER_H