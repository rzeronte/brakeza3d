//
// Created by Eduardo on 23/12/2025.
//

#include "ProjectChecker.h"
#include "../Brakeza.h"
#include "../Misc/ToolsJSON.h"
#include "../GUI/Objects/FileSystemGUI.h"
#include "../Misc/Logging.h"
#include "../Render/Drawable.h"

// =============================================================================
// JSON HELPER FUNCTIONS
// =============================================================================
std::string ProjectChecker::GetStringFromJSON(cJSON* parent, const char* key, const std::string& defaultValue)
{
    auto item = cJSON_GetObjectItemCaseSensitive(parent, key);
    return (item && item->valuestring) ? item->valuestring : defaultValue;
}

int ProjectChecker::GetIntFromJSON(cJSON* parent, const char* key, int defaultValue)
{
    auto item = cJSON_GetObjectItemCaseSensitive(parent, key);
    return item ? item->valueint : defaultValue;
}

float ProjectChecker::GetFloatFromJSON(cJSON* parent, const char* key, float defaultValue)
{
    auto item = cJSON_GetObjectItemCaseSensitive(parent, key);
    return item ? (float)item->valuedouble : defaultValue;
}

Vertex3D ProjectChecker::GetVertex3DFromJSON(cJSON* parent, const char* key)
{
    auto item = cJSON_GetObjectItemCaseSensitive(parent, key);
    return item ? ToolsJSON::getVertex3DByJSON(item) : Vertex3D();
}

// =============================================================================
// PARSING FUNCTIONS
// =============================================================================
ProjectCheckerScript ProjectChecker::ParseScriptFromJSON(cJSON* scriptJSON) const
{
    return {
        GetStringFromJSON(scriptJSON, "name"),
        GetStringFromJSON(scriptJSON, "typesFile"),
        GetStringFromJSON(scriptJSON, "codeFile")
    };
}

ProjectCheckerScene ProjectChecker::ParseSceneFromJSON(cJSON* sceneJSON) const
{
    std::string scenePath = GetStringFromJSON(sceneJSON, "path");
    bool exists = Tools::FileExists(scenePath.c_str());

    return {
        scenePath,
        exists
    };
}

// =============================================================================
// EXTRACTION FUNCTIONS
// =============================================================================
void ProjectChecker::ExtractName(cJSON* json)
{
    status.name = GetStringFromJSON(json, "name", "Unnamed Project");
}

void ProjectChecker::ExtractGravity(cJSON* json)
{
    status.gravity = GetVertex3DFromJSON(json, "gravity");
}

void ProjectChecker::ExtractScripts(cJSON* json)
{
    auto scriptsJSON = cJSON_GetObjectItemCaseSensitive(json, "scripts");
    if (scriptsJSON == nullptr) return;

    cJSON* currentScript;
    cJSON_ArrayForEach(currentScript, scriptsJSON) {
        status.scripts.push_back(ParseScriptFromJSON(currentScript));
    }
}

void ProjectChecker::ExtractScenes(cJSON* json)
{
    auto scenesJSON = cJSON_GetObjectItemCaseSensitive(json, "scenes");
    if (scenesJSON == nullptr) return;

    cJSON* currentScene;
    cJSON_ArrayForEach(currentScene, scenesJSON) {
        status.scenes.push_back(ParseSceneFromJSON(currentScene));
    }
}

void ProjectChecker::ExtractSound(cJSON* json)
{
    auto soundJSON = cJSON_GetObjectItemCaseSensitive(json, "sound");
    if (soundJSON != nullptr) {
        status.sound.volumeFX = GetIntFromJSON(soundJSON, "volume_fx", 50);
        status.sound.volumeMusic = GetIntFromJSON(soundJSON, "volume_music", 50);
    }
}

void ProjectChecker::ExtractResolution(cJSON* json)
{
    auto resolutionJSON = cJSON_GetObjectItemCaseSensitive(json, "resolution");
    if (resolutionJSON != nullptr) {
        status.resolution.width = GetIntFromJSON(resolutionJSON, "width", 1920);
        status.resolution.height = GetIntFromJSON(resolutionJSON, "height", 1080);
    }
}

void ProjectChecker::ExtractProjectInfo(cJSON* json)
{
    ExtractName(json);
    ExtractGravity(json);
    ExtractScripts(json);
    ExtractScenes(json);
    ExtractSound(json);
    ExtractResolution(json);

    loaded = true;
    LOG_MESSAGE("[ProjectChecker] Project loaded successfully: %s", status.name.c_str());
}

// =============================================================================
// DRAWING HELPER FUNCTIONS
// =============================================================================
void ProjectChecker::DrawFileStatus(const std::string& filepath) const
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

// =============================================================================
// DRAWING TABLES
// =============================================================================
void ProjectChecker::DrawInformationTable() const
{
    if (ImGui::BeginTable("ProjectInfoTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        // Project Name
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Project Name:");
        ImGui::TableSetColumnIndex(1);
        ImGui::TextWrapped("%s", status.name.c_str());

        // File Path
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "File Path:");
        ImGui::TableSetColumnIndex(1);
        ImGui::TextWrapped("%s", path.c_str());

        // Gravity
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Gravity:");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f, %.2f, %.2f", status.gravity.x, status.gravity.y, status.gravity.z);

        // Resolution
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Resolution:");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d x %d", status.resolution.width, status.resolution.height);

        // Sound FX Volume
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "FX Volume:");
        ImGui::TableSetColumnIndex(1);
        ImGui::ProgressBar(status.sound.volumeFX / 100.0f, ImVec2(-1, 0),
            (std::to_string(status.sound.volumeFX) + "%").c_str());

        // Sound Music Volume
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Music Volume:");
        ImGui::TableSetColumnIndex(1);
        ImGui::ProgressBar(status.sound.volumeMusic / 100.0f, ImVec2(-1, 0),
            (std::to_string(status.sound.volumeMusic) + "%").c_str());

        ImGui::EndTable();
    }
}

void ProjectChecker::DrawScriptsTable() const
{
    std::string labelScripts = "Project Scripts (" + std::to_string(status.scripts.size()) + ")";
    ImGui::SeparatorText(labelScripts.c_str());

    if (status.scripts.empty()) {
        Drawable::WarningMessage("No scripts in project");
        return;
    }

    if (ImGui::BeginTable("ProjectScriptsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
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

void ProjectChecker::DrawScenesTable() const
{
    std::string labelScenes = "Project Scenes (" + std::to_string(status.scenes.size()) + ")";
    ImGui::SeparatorText(labelScenes.c_str());

    if (status.scenes.empty()) {
        Drawable::WarningMessage("No scenes in project");
        return;
    }

    if (ImGui::BeginTable("ProjectScenesTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < status.scenes.size(); i++) {
            auto &scene = status.scenes[i];
            ImGui::PushID((int)i);

            ImGui::TableNextRow();

            // Status
            ImGui::TableSetColumnIndex(0);
            ImGui::Image(
                FileSystemGUI::Icon(scene.exists ? IconGUI::CHECKED : IconGUI::FILE_BROKEN),
                GUIType::Sizes::ICONS_OBJECTS_ALLOWED
            );

            // Path
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(
                scene.exists ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                "%s",
                scene.path.c_str()
            );

            // Actions
            ImGui::TableSetColumnIndex(2);
            if (scene.exists) {
                if (ImGui::SmallButton("Inspect")) {
                    // Aquí podrías llamar a sceneChecker para inspeccionar la escena
                    SceneChecker sceneChecker;
                    sceneChecker.LoadSceneInfoDialog(scene.path);
                }
            } else {
                ImGui::TextDisabled("Missing");
            }

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void ProjectChecker::DrawWinProjectInfo() const
{
    if (!loaded) return;

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 10.0f));

    DrawInformationTable();
    DrawScriptsTable();
    DrawScenesTable();

    ImGui::PopStyleVar();
}

// =============================================================================
// RESET AND LOAD
// =============================================================================
void ProjectChecker::ResetStatus()
{
    status.name.clear();
    status.gravity = Vertex3D();
    status.scripts.clear();
    status.scenes.clear();
    status.sound = ProjectCheckerSound();
    status.resolution = ProjectCheckerResolution();
    loaded = false;
}

void ProjectChecker::LoadProjectInfoDialog(const std::string& pathFile)
{
    path = pathFile;
    ResetStatus();

    auto json = cJSON_Parse(Tools::ReadFile(pathFile));
    try {
        ExtractProjectInfo(json);
    } catch (const std::exception& e) {
        LOG_ERROR("[ProjectChecker] Cannot open project file!: %s", e.what());
        cJSON_Delete(json);
        return;
    }

    cJSON_Delete(json);
    Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::PROJECT_INFO)->isOpen = true;
}