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
    auto scripts = gui->getBrowserScripts();
    auto scenes = gui->getBrowserScenes();
    auto shaders = gui->getBrowserShaders();
    auto projects = gui->getBrowserProjects();

    scripts.folderFiles = Tools::getFolderFiles(scripts.currentFolder, Config::get()->SCRIPTS_EXT);
    scenes.folderFiles = Tools::getFolderFiles(scripts.currentFolder, Config::get()->SCENES_EXT);
    shaders.folderFiles = Tools::getFolderFiles(scripts.currentFolder, Config::get()->SHADERS_EXT);
    projects.folderFiles = Tools::getFolderFiles(scripts.currentFolder, Config::get()->PROJECTS_EXT);
}

void FileSystemGUI::DrawProjectFiles(GUIManager *gui, GUIType::FolderBrowserCache &browser)
{
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));
    if (ImGui::InputText("Project name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }

    GUI::DrawButton("Create Project", IconGUI::OPEN, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
        if (!gui->currentVariableToCreateCustomShader.empty()) {
            ProjectLoader::CreateProject(browser.currentFolder + gui->currentVariableToCreateCustomShader);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->PROJECTS_EXT);
        }
    });

    ImGui::Separator();
    std::vector<std::string> files = browser.folderFiles;
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
                GUI::DrawButton("Open project", IconGUI::OPEN, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                    ProjectLoader::LoadProject(browser.currentFolder + file);
                });
                ImGui::SameLine();
                if (ImGui::ImageButton(Icon(IconGUI::SAVE), ImVec2(14, 14))) {
                    ImGui::OpenPopup("##Override project");
                }
                ImGui::SetItemTooltip("Override project");
                GUIManager::ShowDeletePopup("##Override project", "Are you sure to override project?", [&] () {
                    ProjectLoader::SaveProject(browser.currentFolder + file);
                });
                ImGui::SameLine();
                if (ImGui::ImageButton(Icon(IconGUI::REMOVE), ImVec2(14, 14))) {
                    ImGui::OpenPopup("##Deleting project");
                }
                ImGui::SetItemTooltip("Delete project");
                GUIManager::ShowDeletePopup("##Deleting project", "Are you sure to delete?", [&] () {
                    ProjectLoader::RemoveProject(browser.currentFolder + file);
                    UpdateFolderFiles(gui);
                });
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawScenesFolder(GUIManager *gui, GUIType::FolderBrowserCache &browser)
{
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));
    if (ImGui::InputText("Scene name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }

    GUI::DrawButton("Create scene", IconGUI::OPEN, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
        if (!gui->currentVariableToCreateCustomShader.empty()) {
            SceneLoader::CreateScene(browser.currentFolder + gui->currentVariableToCreateCustomShader);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCENES_EXT);
        }
    });

    ImGui::Separator();
    DrawBrowserFolders(gui, Config::get()->SCENES_FOLDER, browser, Config::get()->SCENES_EXT);

    ImGui::Separator();

    auto files = browser.folderFiles;
    std::sort(files.begin(), files.end() );
    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (ImGui::BeginTable("ScenesFolderTable", 2, flags)) {
        for (int i = 0; i < files.size(); i++) {
            ImGui::TableNextRow();
            const auto& file = files[i];
            auto fullPath = browser.currentFolder + file;
            ImGui::PushID(std::string(file + std::to_string(i)).c_str());

            auto title = std::to_string(i + 1) + ") " + file;
            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                ImGui::TableSetColumnIndex(0);
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() + 5.0f));
                ImGui::Image(Icon(IconGUI::SCENE), ImVec2(24, 24));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));
                ImGui::Text("%s", title.c_str());

                ImGui::TableSetColumnIndex(1);
                GUI::DrawButton("Load scene", IconGUI::OPEN, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                    SceneLoader::ClearScene();
                    SceneLoader::LoadScene(browser.currentFolder + file);
                });
                ImGui::SameLine();
                if (ImGui::ImageButton(Icon(IconGUI::SAVE), ImVec2(14, 14))) {
                    ImGui::OpenPopup("##Overriding scene");
                }
                ImGui::SetItemTooltip("Override scene");
                GUIManager::ShowDeletePopup("##Overriding scene", "Are you sure to override scene?", [&] () {
                    SceneLoader::SaveScene(browser.currentFolder + file);
                });
                ImGui::SameLine();
                if (ImGui::ImageButton(Icon(IconGUI::REMOVE), ImVec2(14, 14))) {
                    ImGui::OpenPopup("##Deleting scene");
                }
                ImGui::SetItemTooltip("Delete scene");
                GUIManager::ShowDeletePopup("##Deleting scene", "Are you sure to delete?", [browser, file, gui] () {
                    Logging::Message("popup on");
                    SceneLoader::RemoveScene(browser.currentFolder + file);
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
    std::string& folderBase,
    GUIType::FolderBrowserCache &browser,
    std::string ext
)
{
    ImGui::Text("Current: %s", browser.currentFolder.c_str());

    ImGui::Separator();

    if (browser.currentFolder != folderBase) {
        ImGui::Image(Icon(IconGUI::FOLDER), ImVec2(24, 24));
        ImGui::SameLine();
        if (ImGui::Button("..")) {
            browser.currentFolder = Tools::GoBackFromFolder(browser.currentFolder);
            browser.folderFolders = Tools::getFolderFolders(browser.currentFolder);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, ext);
        }
    }

    ImGui::Image(Icon(IconGUI::FOLDER), ImVec2(24, 24));
    ImGui::SameLine();
    ImGui::Text(".");

    for (const auto & i : browser.folderFolders) {
        auto fullPathFolder = browser.currentFolder + i;
        ImGui::Image(Icon(IconGUI::FOLDER), ImVec2(24, 24));
        ImGui::SameLine();
        if (ImGui::Button(i.c_str())) {
            browser.currentFolder = fullPathFolder + "/";
            browser.folderFolders = Tools::getFolderFolders(browser.currentFolder);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, ext);
        }
    }
}

void FileSystemGUI::LoadImagesFolder(GUIManager *gui)
{
    auto images = Tools::getFolderFiles(Config::get()->IMAGES_FOLDER, "png");

    for (const auto& f: images) {
        gui->imagesFolder.addItem(Config::get()->IMAGES_FOLDER + f, f);
    }
}

ImTextureID FileSystemGUI::Icon(GUIType::Sheet coords)
{
    return Brakeza::get()->GUI()->getTextureAtlas()->getTextureByXY(coords.x, coords.y)->getOGLImTexture();
}