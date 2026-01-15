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

    // Transform
    float scale = 1.0f;
    Vertex3D position;
    Vertex3D rotation;

    // Files to validate
    std::string modelPath;
    std::string imagePath;

    // Scripts and shaders
    std::vector<SceneCheckerScript> scripts;
    std::vector<SceneCheckerShader> shaders;

    // Flags
    bool collider = false;
    bool enabled = false;
    bool enableLights = false;

    // Animation (Mesh3D)
    float animationSpeed = 1.0f;
    int indexCurrentAnimation = -1;
    bool boneColliderEnabled = false;

    // Light properties
    bool isLight = false;
    Vertex3D lightAmbient;
    Vertex3D lightDiffuse;
    Vertex3D lightSpecular;
    float lightConstant = 1.0f;
    float lightLinear = 0.09f;
    float lightQuadratic = 0.032f;
    float lightCutOff = 0.0f;
    float lightOuterCutOff = 0.0f;
};

struct SceneCheckerStatus
{
    Vertex3D gravity;
    Vertex3D direction;
    Vertex3D ambient;
    Vertex3D diffuse;
    Vertex3D specular;
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

    // JSON Helper functions
    static std::string GetStringFromJSON(cJSON* parent, const char* key, const std::string& defaultValue = "");
    static int GetIntFromJSON(cJSON* parent, const char* key, int defaultValue = 0);
    static float GetFloatFromJSON(cJSON* parent, const char* key, float defaultValue = 0.0f);
    static Vertex3D GetVertex3DFromJSON(cJSON* parent, const char* key);

    // Extraction functions
    void ExtractScreenshot(cJSON* json);
    void ExtractGravity(cJSON* json);
    void ExtractADSIllumination(cJSON* json);
    void ExtractCamera(cJSON* json);
    void ExtractObjects(cJSON* json);
    void ExtractScripts(cJSON* json);
    void ExtractShaders(cJSON* json);

    // Parsing functions
    SceneCheckerScript ParseScriptFromJSON(cJSON* scriptJSON) const;
    SceneCheckerShader ParseShaderFromJSON(cJSON* shaderJSON) const;

    // Drawing helpers
    void DrawFileStatus(const std::string& filepath) const;
    void DrawCheckbox(bool checked) const;

public:
    explicit SceneChecker() = default;
    ~SceneChecker();

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