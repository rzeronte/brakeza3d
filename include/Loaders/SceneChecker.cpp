//
// Created by Eduardo on 23/12/2025.
//

#include "SceneChecker.h"

#include "../Brakeza.h"
#include "../Misc/ToolsJSON.h"
#include "../Components/Component.h"
#include "../GUI/Objects/FileSystemGUI.h"


void SceneChecker::DrawInformationTable() const {
    ImGui::SeparatorText("Camera information");
    if (ImGui::BeginTable("CameraTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Screenshot", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Information", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        if (screenshot->isLoaded()) {
            float fixedWidth = std::min((int) ImGui::GetContentRegionAvail().x, screenshot->width());
            float height = fixedWidth * ((float) screenshot->height() / (float) screenshot->width());
            ImGui::Image(screenshot->getOGLImTexture(), ImVec2(fixedWidth, height));
        }
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Location path:");
        ImGui::Text("%s", path.c_str());

        ImGui::Separator();

        ImGui::Text("Cam position: %f, %f, %f", status.cameraPosition.x, status.cameraPosition.y, status.cameraPosition.z);
        ImGui::Text("Cam rotation: %f, %f, %f", status.cameraRotation.x, status.cameraRotation.y, status.cameraRotation.z);

        ImGui::Separator();
        ImVec4 ambient = ImVec4(status.ambient.x, status.ambient.y, status.ambient.z, 1.0f);
        ImVec4 diffuse = ImVec4(status.diffuse.x, status.diffuse.y, status.diffuse.z, 1.0f);
        ImVec4 specular = ImVec4(status.specular.x, status.specular.y, status.specular.z, 1.0f);
        ImGui::ColorButton("##color_ambient", ambient, 0, ImVec2(25, 25));
        ImGui::SameLine();
        ImGui::ColorButton("##color_diffuse", diffuse, 0, ImVec2(25, 25));
        ImGui::SameLine();
        ImGui::ColorButton("##color_specular", specular, 0, ImVec2(25, 25));
        ImGui::Text("Direction: %f, %f, %f", status.direction.x, status.direction.y, status.direction.z);

        ImGui::EndTable();
    }
}

void SceneChecker::DrawScriptsTable() const {
    std::string labelScripts = "Scripts (" + std::to_string(status.scripts.size()) + ")";
    ImGui::SeparatorText(labelScripts.c_str());
    if (ImGui::BeginTable("ScriptsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        for (auto &o : status.scripts) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine();
            ImGui::Text("%s", o.name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", o.path.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::Image(FileSystemGUI::Icon(Tools::FileExists(o.path.c_str()) ? IconGUI::CHECKED : IconGUI::UNCHECKED), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        }
        ImGui::EndTable();
    }
}

void SceneChecker::DrawObjectsTable() const {
    // Tabla de Objects
    std::string labelObjects = "Objects (" + std::to_string(status.objects.size()) + ")";
    ImGui::SeparatorText(labelObjects.c_str());
    if (ImGui::BeginTable("ObjectsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Scripts", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Enabled", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Collider", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        for (auto &o : status.objects) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", o.name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Image(FileSystemGUI::Icon(getIconObject(o.type)), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine();
            ImGui::Text("%s", getStringObjectType(o.type).c_str());
            ImGui::TableSetColumnIndex(2);
            for (auto i: o.scripts) {
                ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
                ImGui::SameLine();
            }
            ImGui::TableSetColumnIndex(3);
            ImGui::Image(FileSystemGUI::Icon(o.active ? IconGUI::CHECKED : IconGUI::UNCHECKED), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::TableSetColumnIndex(4);
            ImGui::Image(FileSystemGUI::Icon(o.collider ? IconGUI::CHECKED : IconGUI::UNCHECKED), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        }
        ImGui::EndTable();
    }
}

void SceneChecker::DrawShadersTable() const {
    std::string labelShaderes = "Shaders (" + std::to_string(status.shaders.size()) + ")";
    ImGui::SeparatorText(labelShaderes.c_str());
    if (ImGui::BeginTable("ShadersTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        for (auto &o : status.shaders) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine();
            ImGui::Text("%s", o.name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", o.path.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::Image(FileSystemGUI::Icon(Tools::FileExists(o.path.c_str()) ? IconGUI::CHECKED : IconGUI::UNCHECKED), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        }
        ImGui::EndTable();
    }
}

void SceneChecker::DrawWinSceneInfo() const
{
    if (!loaded) return;

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 10.0f)); // x, y padding

    DrawInformationTable();
    DrawScriptsTable();
    ImGui::Spacing();
    DrawShadersTable();
    ImGui::Spacing();
    DrawObjectsTable();

    ImGui::PopStyleVar();
}

void SceneChecker::ExtractSceneInfo(cJSON *json)
{
    if (screenshot != nullptr) {
        delete screenshot;
    }

    // SCREENSHOT
    if (cJSON_GetObjectItemCaseSensitive(json, "screenshot") != nullptr) {
        screenshot = new Image(cJSON_GetObjectItemCaseSensitive(json, "screenshot")->valuestring);
        status.gravity = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(json, "gravity"));
    }

    // GRAVITY
    if (cJSON_GetObjectItemCaseSensitive(json, "gravity") != nullptr) {
        status.gravity = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(json, "gravity"));
    }

    // ADS ILLUMINATION
    cJSON *adsJSON = cJSON_GetObjectItemCaseSensitive(json, "ads");
    if (adsJSON != nullptr) {
        status.direction = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "direction"));
        status.ambient = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "ambient"));
        status.diffuse = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "diffuse"));
        status.specular = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(adsJSON, "specular"));
    }

    // CAMERA POSITION/ROTATION
    cJSON *cameraJSON = cJSON_GetObjectItemCaseSensitive(json, "camera");
    status.cameraPosition = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(cameraJSON, "position"));
    status.cameraRotation = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(cameraJSON, "rotation"));

    auto objectsJSON = cJSON_GetObjectItemCaseSensitive(json, "objects");
    cJSON *currentObject;
    cJSON_ArrayForEach(currentObject, objectsJSON) {
        SceneCheckerObjectInfo oInfo;

         oInfo.name = cJSON_GetObjectItemCaseSensitive(currentObject, "name")->valuestring,
         oInfo.collider = (bool) cJSON_GetObjectItemCaseSensitive(currentObject, "isCollisionsEnabled")->valueint;
         oInfo.type = (ObjectType) cJSON_GetObjectItemCaseSensitive(currentObject, "type")->valueint;
         oInfo.active = (bool) cJSON_GetObjectItemCaseSensitive(currentObject, "enabled")->valueint;

        // scripts
        if (cJSON_GetObjectItemCaseSensitive(currentObject, "scripts") != nullptr) {
            cJSON *currentScript;
            cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(currentObject, "scripts")) {
                auto name = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
                auto codeFile = cJSON_GetObjectItemCaseSensitive(currentScript, "codeFile")->valuestring;
                auto typesFile = cJSON_GetObjectItemCaseSensitive(currentScript, "typesFile")->valuestring;
                oInfo.scripts.push_back({name, codeFile});
            }
        }
        // shaders
        if (cJSON_GetObjectItemCaseSensitive(currentObject, "shaders") != nullptr) {
            cJSON *currentShader;
            cJSON_ArrayForEach(currentShader, cJSON_GetObjectItemCaseSensitive(currentObject, "shaders")) {
                auto name = cJSON_GetObjectItemCaseSensitive(currentShader, "name")->valuestring;
                auto typesFile = cJSON_GetObjectItemCaseSensitive(currentShader, "typesFile")->valuestring;
                auto codeFile = cJSON_GetObjectItemCaseSensitive(currentShader, "codeFile")->valuestring;

                oInfo.shaders.push_back({name, typesFile});
            }
        }
        status.objects.push_back(oInfo);
    }

    if (cJSON_GetObjectItemCaseSensitive(json, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(json, "scripts")) {
            std::string codeFile = cJSON_GetObjectItemCaseSensitive(currentScript, "codeFile")->valuestring;
            std::string name = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            std::string typesFile = cJSON_GetObjectItemCaseSensitive(currentScript, "typesFile")->valuestring;
            status.scripts.push_back({name, codeFile});
        }
    }

    cJSON *currentShaderJSON;
    cJSON_ArrayForEach(currentShaderJSON, cJSON_GetObjectItemCaseSensitive(json, "shaders")) {
        auto vsFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "vsFile")->valuestring;
        auto fsFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "fsFile")->valuestring;
        auto typesFile = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "typesFile")->valuestring;
        auto dataTypes = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "types");
        auto name = cJSON_GetObjectItemCaseSensitive(currentShaderJSON, "name")->valuestring;
        status.shaders.push_back({name, typesFile});
    }

    loaded = true;
    LOG_MESSAGE("[SceneChecker] Scene checking... %d", loaded);
}

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
    ExtractSceneInfo(json);
    cJSON_Delete(json);

    Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::SCENE_INFO)->isOpen = true;
}

std::string SceneChecker::getStringObjectType(ObjectType type)
{
    auto types = Brakeza::get()->GUI()->getVisibleObjectTypes();

    for (auto &o : types) {
        if (type == o.type) {
            return o.label;
        }
    }

    return std::string("No found");
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
