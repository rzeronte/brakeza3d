//
// Created by Eduardo on 26/11/2025.
//

#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/Brakeza.h"

#include <functional>
#include <string>
#include <vector>

#include "../../../include/Loaders/ProjectLoader.h"
#include "../../../include/Loaders/SceneLoader.h"
#include "../../../include/Misc/Logging.h"
#include "../../../include/Misc/Tools.h"

void FileSystemGUI::UpdateFolderFiles(GUIManager *gui)
{
    Logging::Message("Updating folder files");
    auto scripts = gui->getBrowserScripts();
    auto scenes = gui->getBrowserScenes();
    auto shaders = gui->getBrowserShaders();
    auto projects = gui->getBrowserProjects();

    scripts.folderFiles = Tools::getFolderFiles(scripts.currentFolder, Config::get()->SCRIPTS_EXT);
    scenes.folderFiles = Tools::getFolderFiles(scenes.currentFolder, Config::get()->SCENES_EXT);
    shaders.folderFiles = Tools::getFolderFiles(shaders.currentFolder, Config::get()->SHADERS_EXT);
    projects.folderFiles = Tools::getFolderFiles(projects.currentFolder, Config::get()->PROJECTS_EXT);
}

void FileSystemGUI::DrawProjectFiles(GUIManager *gui, GUIType::FolderBrowserCache &browser)
{
    auto windowStatus = gui->getWindowStatus(GUIType::FILES_PROJECTS);
    if (!windowStatus->isOpen) return;

    GUI::DrawButton("Create Project", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
        if (!gui->currentVariableToCreateCustomShader.empty()) {
            ProjectLoader::CreateProject(browser.currentFolder + gui->currentVariableToCreateCustomShader);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->PROJECTS_EXT);
        }
    });
    ImGui::SameLine();
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));
    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("Project name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }
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
                ImGui::Image(Icon(IconGUI::PROJECT_FILE), GUIType::Sizes::ICONS_BROWSERS);
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));
                ImGui::Text("%s", title.c_str());

                ImGui::TableSetColumnIndex(1);
                GUI::DrawButtonTransparent("Open project", IconGUI::PROJECT_OPEN, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                    ProjectLoader::LoadProject(browser.currentFolder + file);
                });
                ImGui::SameLine();
                GUI::DrawButtonConfirm("Override project", "Are you sure to override project?", IconGUI::SAVE, GUIType::Sizes::ICONS_BROWSERS, [&] {
                    ProjectLoader::SaveProject(browser.currentFolder + file);
                });

                ImGui::SameLine();
                GUI::DrawButtonConfirm("Deleting project", "Are you sure to delete project?", IconGUI::PROJECT_REMOVE, GUIType::Sizes::ICONS_BROWSERS, [&] {
                    ProjectLoader::RemoveProject(browser.currentFolder + file);
                    browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
                });
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawScenesFolder(GUIManager *gui, GUIType::FolderBrowserCache &browser)
{
    auto windowStatus = gui->getWindowStatus(GUIType::FILES_SCENES);
    if (!windowStatus->isOpen) return;

    GUI::DrawButton("Create scene", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
        if (!gui->currentVariableToCreateCustomShader.empty()) {
            SceneLoader::CreateScene(browser.currentFolder + gui->currentVariableToCreateCustomShader);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCENES_EXT);
        }
    });
    ImGui::SameLine();
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));
    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("Scene name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }
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
            ImGui::PushID(i);

            auto title = std::to_string(i + 1) + ") " + file;
            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                ImGui::TableSetColumnIndex(0);
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() + 5.0f));
                ImGui::Image(Icon(IconGUI::SCENE_FILE), GUIType::Sizes::ICONS_BROWSERS);
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));
                ImGui::Text("%s", title.c_str());

                ImGui::TableSetColumnIndex(1);
                GUI::DrawButtonTransparent("Load scene", IconGUI::SCENE_LOAD, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                    SceneLoader::ClearScene();
                    SceneLoader::LoadScene(browser.currentFolder + file);
                });
                ImGui::SameLine();
                GUI::DrawButtonConfirm("Overriding scene", "Are you sure to override scene?", IconGUI::SAVE, GUIType::Sizes::ICONS_BROWSERS, [&] {
                    SceneLoader::SaveScene(browser.currentFolder + file);
                });
                ImGui::SameLine();
                GUI::DrawButtonConfirm("Deleting scene", "Are you sure to delete scene?", IconGUI::SCENE_REMOVE, GUIType::Sizes::ICONS_BROWSERS, [&] {
                    SceneLoader::RemoveScene(browser.currentFolder + file);
                    browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
                });
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawBrowserFolders(
    GUIManager *gui,
    std::string& baseFolder,
    GUIType::FolderBrowserCache &browser,
    std::string ext
)
{
    ImGui::Text("Current: %s", browser.currentFolder.c_str());

    ImGui::Separator();

    if (browser.currentFolder != baseFolder) {
        ImGui::Image(Icon(IconGUI::FOLDER), ImVec2(24, 24));
        ImGui::SameLine();
        if (ImGui::Button("..")) {
            browser.currentFolder = Tools::GoBackFromFolder(browser.currentFolder);
            browser.folderFolders = Tools::getFolderFolders(browser.currentFolder);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, ext);
        }
    }

    ImGui::Image(Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text(".");

    for (const auto & i : browser.folderFolders) {
        auto fullPathFolder = browser.currentFolder + i;
        ImGui::Image(Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_BROWSERS);
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
    auto images = Tools::getFolderFiles(Config::get()->IMAGES_FOLDER, Config::get()->IMAGES_EXT);

    for (const auto& f: images) {
        gui->imagesFolder.addItem(Config::get()->IMAGES_FOLDER + f, f);
    }
}

ImTextureID FileSystemGUI::Icon(GUIType::Sheet coords)
{
    return Brakeza::get()->GUI()->getTextureAtlas()->getTextureByXY(coords.x, coords.y)->getOGLImTexture();
}