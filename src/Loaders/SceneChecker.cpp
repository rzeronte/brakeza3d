//
// Created by Eduardo on 23/12/2025.
//

#include "../../include/Loaders/SceneChecker.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Components/Component.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/Drawable.h"
#include "../include/SceneObjectTypes.h"

// =============================================================================
// DESTRUCTOR
// =============================================================================
SceneChecker::~SceneChecker()
{
    if (screenshot != nullptr) {
        delete screenshot;
        screenshot = nullptr;
    }
}

// =============================================================================
// JSON HELPER FUNCTIONS
// =============================================================================
std::string SceneChecker::GetStringFromJSON(cJSON* parent, const char* key, const std::string& defaultValue)
{
    auto item = cJSON_GetObjectItemCaseSensitive(parent, key);
    return (item && item->valuestring) ? item->valuestring : defaultValue;
}

int SceneChecker::GetIntFromJSON(cJSON* parent, const char* key, int defaultValue)
{
    auto item = cJSON_GetObjectItemCaseSensitive(parent, key);
    return item ? item->valueint : defaultValue;
}

float SceneChecker::GetFloatFromJSON(cJSON* parent, const char* key, float defaultValue)
{
    auto item = cJSON_GetObjectItemCaseSensitive(parent, key);
    return item ? (float)item->valuedouble : defaultValue;
}

Vertex3D SceneChecker::GetVertex3DFromJSON(cJSON* parent, const char* key)
{
    auto item = cJSON_GetObjectItemCaseSensitive(parent, key);
    return item ? ToolsJSON::getVertex3DByJSON(item) : Vertex3D();
}

// =============================================================================
// PARSING FUNCTIONS
// =============================================================================
SceneCheckerScript SceneChecker::ParseScriptFromJSON(cJSON* scriptJSON) const
{
    return {
        GetStringFromJSON(scriptJSON, "name"),
        GetStringFromJSON(scriptJSON, "typesFile"),
        GetStringFromJSON(scriptJSON, "codeFile")
    };
}

SceneCheckerShader SceneChecker::ParseShaderFromJSON(cJSON* shaderJSON) const
{
    return {
        GetStringFromJSON(shaderJSON, "name"),
        GetStringFromJSON(shaderJSON, "vsFile"),
        GetStringFromJSON(shaderJSON, "fsFile"),
        GetStringFromJSON(shaderJSON, "typesFile")
    };
}

// =============================================================================
// EXTRACTION FUNCTIONS
// =============================================================================
void SceneChecker::ExtractScreenshot(cJSON* json)
{
    if (screenshot != nullptr) {
        delete screenshot;
        screenshot = nullptr;
    }

    auto screenshotPath = GetStringFromJSON(json, "screenshot");
    if (!screenshotPath.empty()) {
        screenshot = new Image(screenshotPath);
    }
}

void SceneChecker::ExtractGravity(cJSON* json)
{
    status.gravity = GetVertex3DFromJSON(json, "gravity");
}

void SceneChecker::ExtractADSIllumination(cJSON* json)
{
    auto adsJSON = cJSON_GetObjectItemCaseSensitive(json, "ads");
    if (adsJSON != nullptr) {
        status.direction = GetVertex3DFromJSON(adsJSON, "direction");
        status.ambient = GetVertex3DFromJSON(adsJSON, "ambient");
        status.diffuse = GetVertex3DFromJSON(adsJSON, "diffuse");
        status.specular = GetVertex3DFromJSON(adsJSON, "specular");
    }
}

void SceneChecker::ExtractCamera(cJSON* json)
{
    auto cameraJSON = cJSON_GetObjectItemCaseSensitive(json, "camera");
    if (cameraJSON != nullptr) {
        status.cameraPosition = GetVertex3DFromJSON(cameraJSON, "position");
        status.cameraRotation = GetVertex3DFromJSON(cameraJSON, "rotation");
    }
}

void SceneChecker::ExtractObjects(cJSON* json)
{
    auto objectsJSON = cJSON_GetObjectItemCaseSensitive(json, "objects");
    if (objectsJSON == nullptr) return;

    cJSON* currentObject;
    cJSON_ArrayForEach(currentObject, objectsJSON) {
        SceneCheckerObjectInfo oInfo;

        // Basic info
        oInfo.name = GetStringFromJSON(currentObject, "name");
        oInfo.type = (ObjectType)GetIntFromJSON(currentObject, "type");
        oInfo.enabled = (bool)GetIntFromJSON(currentObject, "enabled");
        oInfo.collider = (bool)GetIntFromJSON(currentObject, "isCollisionsEnabled");

        // Transform
        oInfo.scale = GetFloatFromJSON(currentObject, "scale", 1.0f);
        oInfo.position = GetVertex3DFromJSON(currentObject, "position");
        oInfo.rotation = GetVertex3DFromJSON(currentObject, "rotation");

        // Model path (for Model3D and Mesh3D)
        oInfo.modelPath = GetStringFromJSON(currentObject, "model");

        // Image path (for Image3D)
        oInfo.imagePath = GetStringFromJSON(currentObject, "image");

        // Lights flag
        oInfo.enableLights = (bool)GetIntFromJSON(currentObject, "enableLights");

        // Animation (Mesh3D)
        oInfo.animationSpeed = GetFloatFromJSON(currentObject, "animationSpeed", 1.0f);
        oInfo.indexCurrentAnimation = GetIntFromJSON(currentObject, "indexCurrentAnimation", -1);
        oInfo.boneColliderEnabled = (bool)GetIntFromJSON(currentObject, "boneColliderEnabled");

        // Light properties
        if (oInfo.type == ObjectType::LightPoint || oInfo.type == ObjectType::LightSpot) {
            oInfo.isLight = true;
            oInfo.lightAmbient = GetVertex3DFromJSON(currentObject, "ambient");
            oInfo.lightDiffuse = GetVertex3DFromJSON(currentObject, "diffuse");
            oInfo.lightSpecular = GetVertex3DFromJSON(currentObject, "specular");
            oInfo.lightConstant = GetFloatFromJSON(currentObject, "constant", 1.0f);
            oInfo.lightLinear = GetFloatFromJSON(currentObject, "linear", 0.09f);
            oInfo.lightQuadratic = GetFloatFromJSON(currentObject, "quadratic", 0.032f);

            if (oInfo.type == ObjectType::LightSpot) {
                oInfo.lightCutOff = GetFloatFromJSON(currentObject, "cutOff", 0.0f);
                oInfo.lightOuterCutOff = GetFloatFromJSON(currentObject, "outerCutOff", 0.0f);
            }
        }

        // Extract object scripts
        auto scriptsJSON = cJSON_GetObjectItemCaseSensitive(currentObject, "scripts");
        if (scriptsJSON != nullptr) {
            cJSON* currentScript;
            cJSON_ArrayForEach(currentScript, scriptsJSON) {
                oInfo.scripts.push_back(ParseScriptFromJSON(currentScript));
            }
        }

        // Extract object shaders
        auto shadersJSON = cJSON_GetObjectItemCaseSensitive(currentObject, "shaders");
        if (shadersJSON != nullptr) {
            cJSON* currentShader;
            cJSON_ArrayForEach(currentShader, shadersJSON) {
                oInfo.shaders.push_back(ParseShaderFromJSON(currentShader));
            }
        }

        status.objects.push_back(oInfo);
    }
}

void SceneChecker::ExtractScripts(cJSON* json)
{
    auto scriptsJSON = cJSON_GetObjectItemCaseSensitive(json, "scripts");
    if (scriptsJSON == nullptr) return;

    cJSON* currentScript;
    cJSON_ArrayForEach(currentScript, scriptsJSON) {
        status.scripts.push_back(ParseScriptFromJSON(currentScript));
    }
}

void SceneChecker::ExtractShaders(cJSON* json)
{
    auto shadersJSON = cJSON_GetObjectItemCaseSensitive(json, "shaders");
    if (shadersJSON == nullptr) return;

    cJSON* currentShader;
    cJSON_ArrayForEach(currentShader, shadersJSON) {
        status.shaders.push_back(ParseShaderFromJSON(currentShader));
    }
}

void SceneChecker::ExtractSceneInfo(cJSON* json)
{
    ExtractScreenshot(json);
    ExtractGravity(json);
    ExtractADSIllumination(json);
    ExtractCamera(json);
    ExtractObjects(json);
    ExtractScripts(json);
    ExtractShaders(json);

    loaded = true;
    LOG_MESSAGE("[sceneChecker] Scene loaded successfully");
}

// =============================================================================
// DRAWING HELPER FUNCTIONS
// =============================================================================
void SceneChecker::DrawFileStatus(const std::string& filepath) const
{
    if (filepath.empty()) return;

    bool fileExists = Tools::FileExists(filepath.c_str());
    ImGui::Image(
        FileSystemGUI::Icon(fileExists ? IconGUI::CHECKED : IconGUI::FILE_BROKEN),
        GUIType::Sizes::ICONS_OBJECTS_ALLOWED
    );
    ImGui::SameLine();
    ImGui::TextColored(
        fileExists ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
        "%s",
        filepath.c_str()
    );
}

void SceneChecker::DrawCheckbox(bool checked) const
{
    ImGui::Image(
        FileSystemGUI::Icon(checked ? IconGUI::CHECKED : IconGUI::UNCHECKED),
        GUIType::Sizes::ICONS_OBJECTS_ALLOWED
    );
}

// =============================================================================
// DRAWING TABLES
// =============================================================================
void SceneChecker::DrawInformationTable() const
{
    if (ImGui::BeginTable("InformationTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Screenshot", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Information", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        // Screenshot
        if (screenshot != nullptr && screenshot->isLoaded() && screenshot->getOGLTextureID() != 0) {
            float fixedWidth = std::min((int)ImGui::GetContentRegionAvail().x, screenshot->width());
            float height = fixedWidth * ((float)screenshot->height() / (float)screenshot->width());
            ImGui::Image(screenshot->getOGLImTexture(), ImVec2(fixedWidth, height));
        } else {
            Drawable::WarningMessage("No screenshot available");
        }

        ImGui::TableSetColumnIndex(1);

        // File Path
        ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "File Path:");
        ImGui::TextWrapped("%s", path.c_str());
        ImGui::Separator();

        // Gravity
        ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Gravity:");
        ImGui::Text("%.2f, %.2f, %.2f", status.gravity.x, status.gravity.y, status.gravity.z);
        ImGui::Separator();

        // Camera
        ImGui::Image(FileSystemGUI::Icon(IconGUI::MNU_CAMERA), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Camera:");
        ImGui::Text("Position: %.2f, %.2f, %.2f", status.cameraPosition.x, status.cameraPosition.y, status.cameraPosition.z);
        ImGui::Text("Rotation: %.2f, %.2f, %.2f", status.cameraRotation.x, status.cameraRotation.y, status.cameraRotation.z);
        ImGui::Separator();

        // Directional Light
        ImVec4 ambient = ImVec4(status.ambient.x, status.ambient.y, status.ambient.z, 1.0f);
        ImVec4 diffuse = ImVec4(status.diffuse.x, status.diffuse.y, status.diffuse.z, 1.0f);
        ImVec4 specular = ImVec4(status.specular.x, status.specular.y, status.specular.z, 1.0f);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::TOOLBAR_ENABLE_LIGHT_SYSTEM), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Directional light:");
        ImGui::ColorButton("##color_ambient", ambient, 0, ImVec2(25, 25));
        ImGui::SameLine();
        ImGui::ColorButton("##color_diffuse", diffuse, 0, ImVec2(25, 25));
        ImGui::SameLine();
        ImGui::ColorButton("##color_specular", specular, 0, ImVec2(25, 25));
        ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_SUN_DIRECTION), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::Text("Direction: %.2f, %.2f, %.2f", status.direction.x, status.direction.y, status.direction.z);

        ImGui::EndTable();
    }
}

void SceneChecker::DrawScriptsTable() const
{
    std::string labelScripts = "Scripts (" + std::to_string(status.scripts.size()) + ")";
    ImGui::SeparatorText(labelScripts.c_str());

    if (status.scripts.empty()) {
        Drawable::WarningMessage("No scripts in scene");
        return;
    }

    if (ImGui::BeginTable("ScriptsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Files", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (auto &script : status.scripts) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine();
            ImGui::Text("%s", script.name.c_str());

            ImGui::TableSetColumnIndex(1);
            DrawFileStatus(script.typesFile);
            DrawFileStatus(script.codeFile);
        }
        ImGui::EndTable();
    }
}

void SceneChecker::DrawObjectScriptsTable(const SceneCheckerObjectInfo &o) const
{
    for (auto &script: o.scripts) {
        bool fileExists = Tools::FileExists(script.typesFile.c_str());
        ImGui::Image(FileSystemGUI::Icon(fileExists ? IconGUI::SCRIPT_FILE : IconGUI::FILE_BROKEN), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", script.typesFile.c_str());
        }
        ImGui::SameLine();
    }
}

void SceneChecker::DrawObjectsTable() const
{
    std::string labelObjects = "Objects (" + std::to_string(status.objects.size()) + ")";
    ImGui::SeparatorText(labelObjects.c_str());

    if (status.objects.empty()) {
        Drawable::WarningMessage("No objects in scene");
        return;
    }

    if (ImGui::BeginTable("ObjectsTable", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Transform", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Files", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Scripts", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Lights", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Collider", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < status.objects.size(); i++) {
            auto &o = status.objects[i];
            ImGui::PushID((int)i);

            ImGui::TableNextRow();

            // Name
            ImGui::TableSetColumnIndex(0);

            // Si es luz o animación, hacer expandible
            bool hasDetails = o.isLight || (o.indexCurrentAnimation >= 0);
            if (hasDetails) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
                bool isOpen = ImGui::TreeNodeEx(o.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
                ImGui::PopStyleVar();

                // Type
                ImGui::TableSetColumnIndex(1);
                ImGui::Image(FileSystemGUI::Icon(getIconObject(o.type)), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
                ImGui::SameLine();
                ImGui::Text("%s", getStringObjectType(o.type).c_str());

                // Transform
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("S:%.2f", o.scale);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Pos: %.2f, %.2f, %.2f\nRot: %.2f, %.2f, %.2f",
                        o.position.x, o.position.y, o.position.z,
                        o.rotation.x, o.rotation.y, o.rotation.z);
                }

                // Files
                ImGui::TableSetColumnIndex(3);
                if (!o.modelPath.empty()) {
                    bool modelExists = Tools::FileExists(o.modelPath.c_str());
                    ImGui::Image(
                        FileSystemGUI::Icon(modelExists ? IconGUI::CHECKED : IconGUI::FILE_BROKEN),
                        GUIType::Sizes::ICONS_OBJECTS_ALLOWED
                    );
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%s", o.modelPath.c_str());
                    }
                }
                if (!o.imagePath.empty()) {
                    bool imageExists = Tools::FileExists(o.imagePath.c_str());
                    ImGui::Image(
                        FileSystemGUI::Icon(imageExists ? IconGUI::CHECKED : IconGUI::FILE_BROKEN),
                        GUIType::Sizes::ICONS_OBJECTS_ALLOWED
                    );
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%s", o.imagePath.c_str());
                    }
                }

                // Scripts
                ImGui::TableSetColumnIndex(4);
                DrawObjectScriptsTable(o);

                // Lights
                ImGui::TableSetColumnIndex(5);
                DrawCheckbox(o.enableLights);

                // Enabled
                ImGui::TableSetColumnIndex(6);
                DrawCheckbox(o.enabled);

                // Collider
                ImGui::TableSetColumnIndex(7);
                DrawCheckbox(o.collider);

                // Detalles expandidos
                if (isOpen) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Indent();

                    // Información de luz
                    if (o.isLight) {
                        ImGui::Text("Light Properties:");
                        ImVec4 ambient = ImVec4(o.lightAmbient.x, o.lightAmbient.y, o.lightAmbient.z, 1.0f);
                        ImVec4 diffuse = ImVec4(o.lightDiffuse.x, o.lightDiffuse.y, o.lightDiffuse.z, 1.0f);
                        ImVec4 specular = ImVec4(o.lightSpecular.x, o.lightSpecular.y, o.lightSpecular.z, 1.0f);

                        ImGui::ColorButton("##amb", ambient, 0, ImVec2(20, 20)); ImGui::SameLine();
                        ImGui::ColorButton("##dif", diffuse, 0, ImVec2(20, 20)); ImGui::SameLine();
                        ImGui::ColorButton("##spe", specular, 0, ImVec2(20, 20));

                        ImGui::Text("Constant: %.3f", o.lightConstant);
                        ImGui::Text("Linear: %.3f", o.lightLinear);
                        ImGui::Text("Quadratic: %.3f", o.lightQuadratic);

                        if (o.type == ObjectType::LightSpot) {
                            ImGui::Text("Cut Off: %.3f", o.lightCutOff);
                            ImGui::Text("Outer CutOff: %.3f", o.lightOuterCutOff);
                        }
                    }

                    // Información de animación
                    if (o.indexCurrentAnimation >= 0) {
                        ImGui::Text("Animation Properties:");
                        ImGui::Text("Current Animation: %d", o.indexCurrentAnimation);
                        ImGui::Text("Speed: %.2f", o.animationSpeed);
                        ImGui::Text("Bone Colliders: %s", o.boneColliderEnabled ? "Enabled" : "Disabled");
                    }

                    ImGui::Unindent();
                    ImGui::TreePop();
                }
            } else {
                // Objeto simple sin detalles
                ImGui::Text("%s", o.name.c_str());

                // Type
                ImGui::TableSetColumnIndex(1);
                ImGui::Image(FileSystemGUI::Icon(getIconObject(o.type)), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
                ImGui::SameLine();
                ImGui::Text("%s", getStringObjectType(o.type).c_str());

                // Transform
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("S:%.2f", o.scale);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Pos: %.2f, %.2f, %.2f\nRot: %.2f, %.2f, %.2f",
                        o.position.x, o.position.y, o.position.z,
                        o.rotation.x, o.rotation.y, o.rotation.z);
                }

                // Files
                ImGui::TableSetColumnIndex(3);
                if (!o.modelPath.empty()) {
                    bool modelExists = Tools::FileExists(o.modelPath.c_str());
                    ImGui::Image(
                        FileSystemGUI::Icon(modelExists ? IconGUI::CHECKED : IconGUI::FILE_BROKEN),
                        GUIType::Sizes::ICONS_OBJECTS_ALLOWED
                    );
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%s", o.modelPath.c_str());
                    }
                }
                if (!o.imagePath.empty()) {
                    bool imageExists = Tools::FileExists(o.imagePath.c_str());
                    ImGui::Image(
                        FileSystemGUI::Icon(imageExists ? IconGUI::CHECKED : IconGUI::FILE_BROKEN),
                        GUIType::Sizes::ICONS_OBJECTS_ALLOWED
                    );
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%s", o.imagePath.c_str());
                    }
                }

                // Scripts
                ImGui::TableSetColumnIndex(4);
                DrawObjectScriptsTable(o);

                // Lights
                ImGui::TableSetColumnIndex(5);
                DrawCheckbox(o.enableLights);

                // Enabled
                ImGui::TableSetColumnIndex(6);
                DrawCheckbox(o.enabled);

                // Collider
                ImGui::TableSetColumnIndex(7);
                DrawCheckbox(o.collider);
            }

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void SceneChecker::DrawShadersTable() const
{
    std::string labelShaders = "Shaders (" + std::to_string(status.shaders.size()) + ")";
    ImGui::SeparatorText(labelShaders.c_str());

    if (status.shaders.empty()) {
        Drawable::WarningMessage("No shaders in scene");
        return;
    }

    if (ImGui::BeginTable("ShadersTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        ImGui::TableSetupColumn("Files", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (auto &shader : status.shaders) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine();
            ImGui::Text("%s", shader.name.c_str());

            ImGui::TableSetColumnIndex(1);
            DrawFileStatus(shader.typesFile);
            DrawFileStatus(shader.vsFile);
            DrawFileStatus(shader.fsFile);
        }
        ImGui::EndTable();
    }
}

void SceneChecker::DrawWinSceneInfo() const
{
    if (!loaded) return;

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 10.0f));

    DrawInformationTable();
    DrawScriptsTable();
    DrawShadersTable();
    DrawObjectsTable();

    ImGui::PopStyleVar();
}

// =============================================================================
// RESET AND LOAD
// =============================================================================
void SceneChecker::ResetStatus()
{
    status.gravity = Vertex3D();
    status.direction = Vertex3D();
    status.ambient = Vertex3D();
    status.diffuse = Vertex3D();
    status.specular = Vertex3D();
    status.cameraPosition = Vertex3D();
    status.cameraRotation = Vertex3D();
    status.objects.clear();
    status.scripts.clear();
    status.shaders.clear();
}

void SceneChecker::LoadSceneInfoDialog(const std::string& pathFile)
{
    path = pathFile;
    ResetStatus();

    auto json = cJSON_Parse(Tools::ReadFile(pathFile));

    try {
        ExtractSceneInfo(json);
    } catch (const std::exception& e) {
        LOG_ERROR("[sceneChecker] Cannot open scene file!: %s", e.what());
        cJSON_Delete(json);
        return;
    }

    cJSON_Delete(json);
    Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::SCENE_INFO)->isOpen = true;
}

// =============================================================================
// STATIC HELPERS
// =============================================================================
std::string SceneChecker::getStringObjectType(ObjectType type)
{
    auto types = Brakeza::get()->GUI()->getVisibleObjectTypes();
    for (auto &o : types) {
        if (type == o.type) {
            return o.label;
        }
    }
    return std::string("Unknown");
}

GUIType::Sheet SceneChecker::getIconObject(ObjectType type)
{
    auto types = Brakeza::get()->GUI()->getVisibleObjectTypes();
    for (auto &o : types) {
        if (type == o.type) {
            return o.icon;
        }
    }
    return GUIType::Sheet{0, 0};
}