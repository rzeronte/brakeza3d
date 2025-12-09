//
// Created by Eduardo on 26/11/2025.
//

#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/Brakeza.h"
#include "../../../include/Components/Components.h"

#include <functional>
#include <string>
#include <vector>

#include "../../../include/Loaders/ProjectLoader.h"
#include "../../../include/Loaders/SceneLoader.h"
#include "../../../include/Misc/Logging.h"
#include "../../../include/Misc/Tools.h"

void FileSystemGUI::DrawProjectCreator(GUIManager *gui, GUIType::BrowserCache &browser)
{
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));

    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("Project name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }
}

void FileSystemGUI::DrawProjectsTable(GUIManager *gui, GUIType::BrowserCache &browser)
{
    std::vector<std::string> files = browser.folderFiles;
    std::sort(files.begin(), files.end());

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("ProjectsFolderTable", 2, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < files.size(); i++) {
            const auto& file = files[i];
            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                DrawProjectRow(gui, browser, file, i);
            }
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawProjectRowActions(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file)
{
    ImGui::TableSetColumnIndex(1);

    auto fullPath = browser.currentFolder + file;

    GUI::DrawButtonTransparent(
        "Open project",
        IconGUI::PROJECT_OPEN,
        GUIType::Sizes::ICONS_BROWSERS,
        true,
        [&] {
            ProjectLoader::LoadProject(fullPath);
        }
    );
    ImGui::SameLine();
    GUI::DrawButtonConfirm(
        "Override project",
        "Are you sure to override project?",
        IconGUI::SAVE,
        GUIType::Sizes::ICONS_BROWSERS,
        [&] {
            ProjectLoader::SaveProject(fullPath);
        }
    );
    ImGui::SameLine();
    GUI::DrawButtonConfirm(
        "Deleting project",
        "Are you sure to delete project?",
        IconGUI::PROJECT_REMOVE,
        GUIType::Sizes::ICONS_BROWSERS,
        [&] {
            ProjectLoader::RemoveProject(fullPath);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
        }
    );
}

void FileSystemGUI::DrawProjectRow(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file, int index)
{
    ImGui::PushID(index);
    ImGui::TableNextRow();

    // name
    ImGui::TableSetColumnIndex(0);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() + 5.0f));
    ImGui::Image(Icon(IconGUI::PROJECT_FILE), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));
    auto title = std::to_string(index + 1) + ") " + file;
    ImGui::Text("%s", title.c_str());
    ImGui::TableSetColumnIndex(1);
    DrawProjectRowActions(gui, browser, file);

    ImGui::PopID();
}

void FileSystemGUI::DrawProjectFiles(GUIManager *gui, GUIType::BrowserCache &browser)
{
    auto windowStatus = gui->getWindowStatus(GUIType::FILES_PROJECTS);
    if (!windowStatus->isOpen) return;

    static bool openPopup = false;
    if (ImGui::BeginTable("ProjectHeader", 2, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableNextRow();

        // Izquierda
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(Icon(IconGUI::PROJECT_FILE), GUIType::Sizes::ICON_BROWSER_TYPE);
        ImGui::SameLine();
        ImGui::Text("Current: %s", browser.currentFolder.c_str());
        // Derecha
        ImGui::TableSetColumnIndex(1);
        GUI::DrawButton("Create Project", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            openPopup = true;
        });
        ImGui::EndTable();
    }
    static std::string title = "Create project dialog";
    if (openPopup) {
        ImGui::OpenPopup(title.c_str());
        openPopup = false;
    }

    DrawProjectCreatorDialog(gui, browser, title);
    ImGui::Separator();
    DrawProjectsTable(gui, browser);
}

void FileSystemGUI::DrawProjectCreatorDialog(GUIManager *gui, GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create a new project");
        ImGui::Separator();
        DrawProjectCreator(gui, browser);
        ImGui::Separator();
        GUI::DrawButton("Cancel create project", IconGUI::CANCEL, GUIType::Sizes::ICONS_BROWSERS, true,[&] {
            gui->currentVariableToCreateCustomShader = "";
            ImGui::CloseCurrentPopup();
        });
        ImGui::SameLine();
        GUI::DrawButton(
            "Create Project",
            IconGUI::CREATE_FILE,
            GUIType::Sizes::ICONS_BROWSERS,
            true,
    [&] {
                if (!gui->currentVariableToCreateCustomShader.empty()) {
                    ProjectLoader::CreateProject(browser.currentFolder + gui->currentVariableToCreateCustomShader);
                    browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->PROJECTS_EXT);
                    ImGui::CloseCurrentPopup();
                }
            }
        );
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawSceneCreator(GUIManager *gui, GUIType::BrowserCache &browser)
{
    ImGui::SameLine();
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));

    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("Scene name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }
}

void FileSystemGUI::DrawScenesTable(GUIManager *gui, GUIType::BrowserCache &browser)
{
    auto files = browser.folderFiles;
    std::sort(files.begin(), files.end());

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("ScenesFolderTable", 2, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < files.size(); i++) {
            const auto& file = files[i];
            if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                DrawSceneRow(gui, browser, file, i);
            }
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawSceneRow(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file, int index)
{
    ImGui::PushID(index);
    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() + 5.0f));
    ImGui::Image(Icon(IconGUI::SCENE_FILE), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY()));
    auto title = std::to_string(index + 1) + ") " + file;
    ImGui::Text("%s", title.c_str());

    ImGui::TableSetColumnIndex(1);
    DrawSceneRowActions(gui, browser, file);

    ImGui::PopID();
}

void FileSystemGUI::DrawSceneRowActions(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file)
{
    auto fullPath = browser.currentFolder + file;

    // Load button
    GUI::DrawButtonTransparent(
        "Load scene",
        IconGUI::SCENE_LOAD,
        GUIType::Sizes::ICONS_BROWSERS,
        true,
        [&] {
            SceneLoader::ClearScene();
            SceneLoader::LoadScene(fullPath);
        }
    );

    ImGui::SameLine();

    // Save button
    GUI::DrawButtonConfirm(
        "Overriding scene",
        "Are you sure to override scene?",
        IconGUI::SAVE,
        GUIType::Sizes::ICONS_BROWSERS,
        [&] {
            SceneLoader::SaveScene(fullPath);
        }
    );

    ImGui::SameLine();

    // Delete button
    GUI::DrawButtonConfirm(
        "Deleting scene",
        "Are you sure to delete scene?",
        IconGUI::SCENE_REMOVE,
        GUIType::Sizes::ICONS_BROWSERS,
        [&] {
            SceneLoader::RemoveScene(fullPath);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
        }
    );
}

void FileSystemGUI::DrawSceneFiles(GUIManager *gui, GUIType::BrowserCache &browser)
{
    auto windowStatus = gui->getWindowStatus(GUIType::FILES_SCENES);
    if (!windowStatus->isOpen) return;

    static bool openPopup = false;
    if (ImGui::BeginTable("SceneHeader", 2, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableNextRow();
        // Izquierda
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(Icon(IconGUI::SCENE_FILE), GUIType::Sizes::ICON_BROWSER_TYPE);
        ImGui::SameLine();
        ImGui::Text("Current: %s", browser.currentFolder.c_str());

        // Derecha
        ImGui::TableSetColumnIndex(1);
        GUI::DrawButton("Create Scene", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            openPopup = true;
        });

        ImGui::EndTable();
    }
    static std::string title = "Create scene dialog";
    if (openPopup) {
        ImGui::OpenPopup(title.c_str());
        openPopup = false;
    }
    DrawSceneCreatorDialog(gui, browser, title);

    ImGui::Separator();
    DrawBrowserFolders(gui, Config::get()->SCENES_FOLDER, browser, Config::get()->SCENES_EXT);
    ImGui::Separator();
    DrawScenesTable(gui, browser);
}

void FileSystemGUI::DrawSceneCreatorDialog(GUIManager *gui, GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        DrawSceneCreator(gui, browser);
        ImGui::Separator();
        GUI::DrawButton("Cancel create scene", IconGUI::CANCEL, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            gui->currentVariableToCreateCustomShader = "";
            ImGui::CloseCurrentPopup();
        });
        ImGui::SameLine();
        GUI::DrawButton(
            "Create scene",
            IconGUI::CREATE_FILE,
            GUIType::Sizes::ICONS_BROWSERS,
            true,
            [&] {
                if (!gui->currentVariableToCreateCustomShader.empty()) {
                    SceneLoader::CreateScene(browser.currentFolder + gui->currentVariableToCreateCustomShader);
                    browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCENES_EXT);
                    ImGui::CloseCurrentPopup();
                }
            }
        );
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawBrowserFolders(GUIManager *gui, std::string& baseFolder, GUIType::BrowserCache &browser, std::string ext)
{
    if (browser.currentFolder != baseFolder) {
        ImGui::AlignTextToFramePadding();
        ImGui::Image(Icon(IconGUI::FOLDER), ImVec2(24, 24));
        ImGui::SameLine();
        if (ImGui::Button("..")) {
            browser.currentFolder = Tools::GoBackFromFolder(browser.currentFolder);
            browser.folderFolders = Tools::getFolderFolders(browser.currentFolder);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, ext);
        }
    }

    // Para el icono de la carpeta actual
    ImVec2 iconSize = GUIType::Sizes::ICONS_BROWSERS;
    float frameHeight = ImGui::GetFrameHeight();
    float offsetY = (frameHeight - iconSize.y) * 0.5f;

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
    ImGui::Image(Icon(IconGUI::FOLDER), iconSize);
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);
    ImGui::Text(".");

    for (const auto & i : browser.folderFolders) {
        auto fullPathFolder = browser.currentFolder + i;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
        ImGui::Image(Icon(IconGUI::FOLDER), iconSize);
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);
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

std::vector<const char*> FileSystemGUI::GetShaderTypeItems()
{
    std::vector<const char*> items;
    for (const auto& pair : Components::get()->Render()->getShaderTypesMapping()) {
        items.push_back(pair.first.c_str());
    }
    return items;
}

void FileSystemGUI::DrawShaderTypeCombo(int &selectedIndex, const std::vector<const char*> &items)
{
    ImGui::SetNextItemWidth(150);
    if (ImGui::Combo("Type", &selectedIndex, items.data(), items.size())) {
        std::cout << "Seleccionado: " << items[selectedIndex] << std::endl;
    }
}

void FileSystemGUI::DrawShadersTable(GUIManager *gui, GUIType::BrowserCache &browser)
{
    auto files = browser.folderFiles;

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("ScriptsFolderTable", 4, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Load", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < files.size(); i++) {
            DrawShaderRow(gui, browser, files[i], i);
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawShaderRow(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file, int index)
{
    ImGui::PushID(index);
    ImGui::TableNextRow();

    // name
    ImGui::TableSetColumnIndex(0);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_FILE), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    std::string optionText = std::to_string(index + 1) + ") " + file;
    ImGui::Selectable(optionText.c_str());
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        static std::string folderCopy, fileCopy;
        folderCopy = browser.currentFolder;
        fileCopy = file;

        Config::DragDropCustomShaderData data = {folderCopy.c_str(), fileCopy.c_str()};
        ImGui::SetDragDropPayload("CUSTOMSHADER_ITEM", &data, sizeof(data));

        auto fullPath = browser.currentFolder + file;
        ImGui::Text("%s", fullPath.c_str());
        ImGui::EndDragDropSource();
    }

    DrawShaderRowActions(gui, browser, file);

    ImGui::PopID();
}

void FileSystemGUI::DrawShaderRowActions(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file)
{
    auto fullPath = browser.currentFolder + file;

    // Load button
    ImGui::TableSetColumnIndex(1);
    GUI::DrawButtonTransparent(
        "Load shader into scene",
        IconGUI::SHADER_LOAD,
        GUIType::Sizes::ICONS_BROWSERS,
        true,
        [&] {
            Components::get()->Render()->loadShaderIntoScene(browser.currentFolder, file);
        }
    );

    // Edit button
    ImGui::TableSetColumnIndex(2);
    GUI::DrawButton(
        "Edit shader",
        IconGUI::SHADER_EDIT,
        GUIType::Sizes::ICONS_BROWSERS,
        false,
        [&] {
            ShadersGUI::LoadShaderDialog(gui, browser.currentFolder, file);
        }
    );

    // Delete button
    ImGui::TableSetColumnIndex(3);
    GUI::DrawButtonConfirm(
        "Deleting shader",
        "Are you sure to delete shader?",
        IconGUI::SHADER_REMOVE,
        GUIType::Sizes::ICONS_BROWSERS,
        [&] {
            ShaderOGLCustom::RemoveCustomShaderFiles(
                browser.currentFolder,
                Tools::getFilenameWithoutExtension(file)
            );
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
        }
    );
}

void FileSystemGUI::DrawShaderCreator(GUIManager *gui, int &item_current_idx, const std::vector<const char*> &items)
{
    static char name[256];
    strncpy(name, gui->currentVariableToCreateCustomShader.c_str(), sizeof(name));

    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("Shader name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToCreateCustomShader = name;
    }
    DrawShaderTypeCombo(item_current_idx, items);
}

void FileSystemGUI::DrawShaderFiles(GUIManager *gui, GUIType::BrowserCache &browser)
{
    auto windowStatus = gui->getWindowStatus(GUIType::FILES_SHADERS);
    if (!windowStatus->isOpen) return;

        static bool openPopup = false;
    if (ImGui::BeginTable("ShaderHeader", 2, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableNextRow();
        // Izquierda
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(Icon(IconGUI::SHADER_FILE), GUIType::Sizes::ICON_BROWSER_TYPE);
        ImGui::SameLine();
        ImGui::Text("Current: %s", browser.currentFolder.c_str());
        // Derecha
        ImGui::TableSetColumnIndex(1);
        GUI::DrawButton("Create Shader", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            openPopup = true;
        });
        ImGui::EndTable();
    }
    static std::string title = "Create shader dialog";
    if (openPopup) {
        ImGui::OpenPopup(title.c_str());
        openPopup = false;
    }

    DrawShaderCreatorDialog(gui, browser, title);
    ImGui::Separator();
    DrawBrowserFolders(gui, Config::get()->CUSTOM_SHADERS_FOLDER, browser, Config::get()->SHADERS_EXT);
    ImGui::Separator();
    DrawShadersTable(gui, browser);
}

void FileSystemGUI::DrawShaderCreatorDialog(GUIManager *gui, GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static int item_current_idx = 0;
        auto items = GetShaderTypeItems();
        DrawShaderCreator(gui, item_current_idx, items);
        ImGui::Separator();
        GUI::DrawButton("Cancel create shader", IconGUI::CANCEL, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            gui->currentVariableToCreateCustomShader = "";
            ImGui::CloseCurrentPopup();
        });
        ImGui::SameLine();
        GUI::DrawButton("Create shader", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            if (!gui->currentVariableToCreateCustomShader.empty()) {
                auto type = ShaderOGLCustom::getShaderTypeFromString(items[item_current_idx]);
                ShaderOGLCustom::createEmptyCustomShader(
                    gui->currentVariableToCreateCustomShader,
                    browser.currentFolder,
                    type
                );
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
                ImGui::CloseCurrentPopup();
            }
        });
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawScriptCreator(GUIManager *gui, GUIType::BrowserCache &browser)
{
    static char name[256];
    strncpy(name, gui->currentVariableToAddName.c_str(), sizeof(name));

    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("Script name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        gui->currentVariableToAddName = name;
    }
}

void FileSystemGUI::DrawScriptsTable(GUIManager *gui, GUIType::BrowserCache &browser)
{
    auto files = browser.folderFiles;

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("ScriptsFolderTable", 3, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < files.size(); i++) {
            DrawScriptRow(gui, browser, files[i], i);
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawScriptRow(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file, int index)
{
    ImGui::PushID(index);
    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
    ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_BROWSERS);

    ImGui::SameLine();
    std::string optionText = std::to_string(index + 1) + ") " + file;
    ImGui::Selectable(optionText.c_str());
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        auto fullPath = browser.currentFolder + file;
        ImGui::SetDragDropPayload("SCRIPT_ITEM", fullPath.c_str(), fullPath.size() + 1);
        ImGui::Text("%s", fullPath.c_str());
        ImGui::EndDragDropSource();
    }
    DrawScriptRowActions(gui, browser, file);

    ImGui::PopID();
}

void FileSystemGUI::DrawScriptRowActions(GUIManager *gui, GUIType::BrowserCache &browser, const std::string &file)
{
    auto fullPath = browser.currentFolder + file;

    ImGui::TableSetColumnIndex(1);
    GUI::DrawButton(
        "Edit script",
        IconGUI::SCRIPT_EDIT,
        GUIType::Sizes::ICONS_BROWSERS,
        false,
        [&] {
            ScriptLuaGUI::LoadScriptDialog(gui, fullPath);
        }
    );
    ImGui::TableSetColumnIndex(2);
    GUI::DrawButtonConfirm(
        "Deleting script",
        "Are you sure to delete script?",
        IconGUI::SCRIPT_REMOVE,
        GUIType::Sizes::ICONS_BROWSERS,
        [&] {
            ComponentScripting::removeScriptLUAFile(fullPath);
            browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCRIPTS_EXT);
        }
    );
}

void FileSystemGUI::DrawScriptCreatorDialog(GUIManager *gui, GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        DrawScriptCreator(gui, browser);
        ImGui::Separator();
        GUI::DrawButton("Cancel create script", IconGUI::CANCEL, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            gui->currentVariableToAddName = "";
            ImGui::CloseCurrentPopup();
        });
        ImGui::SameLine();
        GUI::DrawButton("Create script", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            if (!gui->currentVariableToAddName.empty()) {
                ComponentScripting::createScriptLUAFile(browser.currentFolder + gui->currentVariableToAddName);
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCRIPTS_EXT);
                ImGui::CloseCurrentPopup();
            }
        });
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawScriptFiles(GUIManager *gui, GUIType::BrowserCache &browser)
{
    auto windowStatus = gui->getWindowStatus(GUIType::FILES_SCRIPTS);
    if (!windowStatus->isOpen) return;

    static bool openPopup = false;
    if (ImGui::BeginTable("ScriptHeader", 2, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableNextRow();
        // Izquierda
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICON_BROWSER_TYPE);
        ImGui::SameLine();
        ImGui::Text("Current: %s", browser.currentFolder.c_str());
        // Derecha
        ImGui::TableSetColumnIndex(1);
        GUI::DrawButton("Create Script", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            openPopup = true;
        });
        ImGui::EndTable();
    }
    static std::string title = "Create script dialog";
    if (openPopup) {
        ImGui::OpenPopup(title.c_str());
        openPopup = false;
    }
    DrawScriptCreatorDialog(gui, browser, title);
    ImGui::Separator();
    DrawBrowserFolders(gui, Config::get()->SCRIPTS_FOLDER, browser, Config::get()->SCRIPTS_EXT);
    ImGui::Separator();
    DrawScriptsTable(gui, browser);
}