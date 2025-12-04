//
// Created by Eduardo on 26/11/2025.
//

#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/Brakeza.h"

#include <functional>
#include <string>
#include <vector>

void FileSystemGUI::UpdateFolderFiles(GUIManager *gui)
{
    gui->currentScriptsFolderFiles = Tools::getFolderFiles(gui->currentScriptsFolderWidget, "lua");
    gui->currentScenesFolderFiles = Tools::getFolderFiles(gui->currentScenesFolderWidget, "json");
    gui->currentProjectsFolderFiles = Tools::getFolderFiles(gui->currentProjectsFolderWidget, "json");
    gui->currentShadersFolderFiles = Tools::getFolderFiles(gui->currentShadersFolderWidget, "json");
}

void FileSystemGUI::DrawProjectFiles(GUIManager *gui, const std::string& folder)
{
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));
    if (ImGui::InputText("Project name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }
    if (ImGui::ImageButton(Icon(IconGUI::OPEN), ImVec2(14, 14))) {
        if (!gui->currentVariableToCreateCustomShader.empty()) {
            ProjectLoader::CreateProject(folder + gui->currentVariableToCreateCustomShader);
            gui->currentProjectsFolderFiles = Tools::getFolderFiles(gui->currentProjectsFolderWidget, "json");
        }
    }

    ImGui::Separator();

    std::vector<std::string> files = gui->currentProjectsFolderFiles;
    std::sort(files.begin(), files.end() );
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ProjectsFolderTable", 2, flags)) {
        for (int i = 0; i < files.size(); i++) {
            ImGui::TableNextRow();
            const auto& file = files[i];
            auto title = std::to_string(i + 1) + ") " + file;
            ImGui::PushID(i);

            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                ImGui::TableSetColumnIndex(0);
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() + 5.0f));
                ImGui::Image(Icon(IconGUI::PROJECT), ImVec2(24, 24));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));
                ImGui::Text("%s", title.c_str());

                ImGui::TableSetColumnIndex(1);
                if (ImGui::ImageButton(Icon(IconGUI::OPEN), ImVec2(24, 24))) {
                    ComponentsManager::get()->getComponentRender()->getProjectLoader().LoadProject(folder + file);
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(Icon(IconGUI::SAVE), ImVec2(24, 24))) {
                    ProjectLoader::SaveProject(folder + file);
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(Icon(IconGUI::REMOVE), ImVec2(24, 24))) {
                    ImGui::OpenPopup("Deleting project");
                }
                gui->ShowDeletePopup("Deleting project", "Are you sure to delete?", [folder, file, gui] () {
                    ProjectLoader::RemoveProject(folder + file);
                    UpdateFolderFiles(gui);
                });

            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawScenesFolder(GUIManager *gui, const std::string& folder)
{
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));
    if (ImGui::InputText("Scene name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }

    if (ImGui::ImageButton(Icon(IconGUI::OPEN), ImVec2(14, 14))) {
        if (!gui->currentVariableToCreateCustomShader.empty()) {
            SceneLoader::CreateScene(folder + gui->currentVariableToCreateCustomShader);
            gui->currentScenesFolderFiles = Tools::getFolderFiles(gui->currentScenesFolderWidget, "json");
        }
    }

    ImGui::Separator();

    DrawBrowserFolders(
        gui,
        folder,
        BrakezaSetup::get()->SCENES_FOLDER,
        gui->currentScenesFolderWidget,
        gui->currentScenesFolders,
        gui->currentScenesFolderFiles,
        "json"
    );

    ImGui::Separator();

    auto files = gui->currentScenesFolderFiles;
    std::sort(files.begin(), files.end() );
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ScenesFolderTable", 2, flags)) {
        for (int i = 0; i < files.size(); i++) {
            ImGui::TableNextRow();
            const auto& file = files[i];
            auto fullPath = folder + file;
            ImGui::PushID(i);

            auto title = std::to_string(i + 1) + ") " + file;
            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                ImGui::TableSetColumnIndex(0);
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() + 5.0f));
                ImGui::Image(Icon(IconGUI::SCENE), ImVec2(24, 24));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));
                ImGui::Text("%s", title.c_str());

                ImGui::TableSetColumnIndex(1);
                if (ImGui::ImageButton(Icon(IconGUI::OPEN), ImVec2(14, 14))) {
                    SceneLoader::ClearScene();
                    ComponentsManager::get()->getComponentRender()->getSceneLoader().LoadScene(folder + file);
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(Icon(IconGUI::SAVE), ImVec2(14, 14))) {
                    ImGui::OpenPopup("Overriding scene");
                }
                GUIManager::ShowDeletePopup("Overriding scene", "Are you sure to override?", [folder, file, gui] () {
                    SceneLoader::SaveScene(folder + file);
                });

                ImGui::SameLine();
                if (ImGui::ImageButton(Icon(IconGUI::REMOVE), ImVec2(14, 14))) {
                    ImGui::OpenPopup("Deleting scene");
                }
                gui->ShowDeletePopup("Deleting scene", "Are you sure to delete?", [folder, file, gui] () {
                    SceneLoader::RemoveScene(folder + file);
                    UpdateFolderFiles(gui);
                });
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawBrowserFolders(
    GUIManager *gui,
    const std::string& folder,
    const std::string& folderBase,
    std::string& destiny,
    std::vector<std::string> &folders,
    std::vector<std::string> &files,
    const std::string& ext
)
{
    ImGui::Text("Current: %s", folder.c_str());

    ImGui::Separator();

    if (folder != folderBase) {
        ImGui::Image(Icon(IconGUI::FOLDER), ImVec2(24, 24));
        ImGui::SameLine();
        if (ImGui::Button("..")) {
            destiny = Tools::GoBackFromFolder(destiny);
            folders = Tools::getFolderFolders(destiny);
            files = Tools::getFolderFiles(destiny, ext);
        }
    }

    ImGui::Image(Icon(IconGUI::FOLDER), ImVec2(24, 24));
    ImGui::SameLine();
    ImGui::Text(".");

    for (const auto & i : folders) {
        auto fullPathFolder = folder + i;
        ImGui::Image(Icon(IconGUI::FOLDER), ImVec2(24, 24));
        ImGui::SameLine();
        if (ImGui::Button(i.c_str())) {
            destiny = fullPathFolder + "/";
            folders = Tools::getFolderFolders(destiny);
            files = Tools::getFolderFiles(destiny, ext);
        }
    }
}

void FileSystemGUI::LoadImagesFolder(GUIManager *gui)
{
    auto images = Tools::getFolderFiles(BrakezaSetup::get()->IMAGES_FOLDER, "png");

    for (const auto& f: images) {
        gui->imagesFolder.addItem(BrakezaSetup::get()->IMAGES_FOLDER + f, f);
    }
}

ImTextureID FileSystemGUI::Icon(GUISheet coords)
{
    return Brakeza::get()->GUI()->getTextureAtlas()->getTextureByXY(coords.x, coords.y)->getOGLImTexture();
}