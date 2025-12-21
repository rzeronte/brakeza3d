//
// Created by Eduardo on 26/11/2025.
//

#include <string>
#include <vector>
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Loaders/ProjectLoader.h"
#include "../../../include/Loaders/SceneLoader.h"
#include "../../../include/Misc/Tools.h"

void FileSystemGUI::DrawProjectCreator()
{

}

void FileSystemGUI::DrawProjectsTable(GUIType::BrowserCache &browser)
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
                DrawProjectRow(browser, file, i);
            }
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawProjectRowActions(GUIType::BrowserCache &browser, const std::string &file)
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

void FileSystemGUI::DrawProjectRow(GUIType::BrowserCache &browser, const std::string &file, int index)
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
    DrawProjectRowActions(browser, file);

    ImGui::PopID();
}

void FileSystemGUI::DrawProjectFiles(GUIType::BrowserCache &browser)
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::FILES_PROJECTS);
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

    DrawProjectCreatorDialog(browser, title);
    ImGui::Separator();
    DrawProjectsTable(browser);
}

void FileSystemGUI::DrawProjectCreatorDialog(GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create a new project");
        ImGui::Separator();
        static char localVarName[256] = "";
        ImGui::SetNextItemWidth(150);
        ImGui::InputText("Project name##", localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);
        ImGui::Separator();

        float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalWidth = buttonWidth * 2 + spacing;

        float availWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalWidth);

        GUI::DrawButton("Cancel create project", IconGUI::CANCEL, GUIType::Sizes::ICONS_BROWSERS, true,[&] {
            localVarName[0] = '\0';
            ImGui::CloseCurrentPopup();
        });
        ImGui::SameLine();
        GUI::DrawButton(
            "Create Project",
            IconGUI::CREATE_FILE,
            GUIType::Sizes::ICONS_BROWSERS,
            true,
            [&] {
                if (localVarName[0] != '\0') {
                    ProjectLoader::CreateProject(browser.currentFolder + localVarName);
                    browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->PROJECTS_EXT);
                    ImGui::CloseCurrentPopup();
                }
            }
        );
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawSceneCreator()
{
}

void FileSystemGUI::DrawScenesTable(GUIType::BrowserCache &browser)
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
                DrawSceneRow(browser, file, i);
            }
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawSceneRow(GUIType::BrowserCache &browser, const std::string &file, int index)
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
    DrawSceneRowActions(browser, file);

    ImGui::PopID();
}

void FileSystemGUI::DrawSceneRowActions(GUIType::BrowserCache &browser, const std::string &file)
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

void FileSystemGUI::DrawSceneFiles(GUIType::BrowserCache &browser)
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::FILES_SCENES);
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
    DrawSceneCreatorDialog(browser, title);

    ImGui::Separator();
    DrawBrowserFolders(Config::get()->SCENES_FOLDER, browser, Config::get()->SCENES_EXT);
    ImGui::Separator();
    DrawScenesTable(browser);
}

void FileSystemGUI::DrawSceneCreatorDialog(GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::SameLine();
        static char localVarName[256] = "";

        ImGui::SetNextItemWidth(150);
        ImGui::InputText("Scene name##", localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);
        ImGui::Separator();

        // Calcular ancho de los botones
        float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalWidth = buttonWidth * 2 + spacing;

        // Alinear a la derecha
        float availWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalWidth);

        GUI::DrawButton("Cancel create scene", IconGUI::CANCEL, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            localVarName[0] = '\0';
            ImGui::CloseCurrentPopup();
        });
        ImGui::SameLine();
        GUI::DrawButton(
            "Create scene",
            IconGUI::CREATE_FILE,
            GUIType::Sizes::ICONS_BROWSERS,
            true,
            [&] {
                if (localVarName[0] != '\0') {
                    SceneLoader::CreateScene(browser.currentFolder + localVarName);
                    browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCENES_EXT);
                    ImGui::CloseCurrentPopup();
                }
            }
        );
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawBrowserFolders(std::string& folder, GUIType::BrowserCache &browser, const std::string &ext)
{
    if (browser.currentFolder != folder) {
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

void FileSystemGUI::DrawCodeEditorTab(EditableOpenFile &file, int tabIndex)
{
    std::string uniqueTabId = file.getTabLabel() + "##" + file.getPath();

    auto currentIndexTab = Brakeza::get()->GUI()->getIndexCodeEditorTab();

    ImGuiTabItemFlags flags = (currentIndexTab == tabIndex) ? ImGuiTabItemFlags_SetSelected : 0;
    if (ImGui::BeginTabItem(uniqueTabId.c_str(), nullptr, flags)) {
        static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
        std::string tableId = "codeEditorTab##" + file.getPath();
        if (ImGui::BeginTable(tableId.c_str(), 2, flags)) {
            ImGui::TableSetupColumn("Setup", ImGuiTableColumnFlags_WidthFixed, 350.0f);
            ImGui::TableSetupColumn("Code", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            // Config File
            ImGui::TableSetColumnIndex(0);
            file.DrawEditableOpenFileConfig();
            // Code Editor
            ImGui::TableSetColumnIndex(1);
            file.DrawCodeEditActionButtons();
            file.getEditor().Render("##editor", ImVec2(0, 0), false);
            ImGui::EndTable();
        }
        ImGui::EndTabItem();
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

std::vector<const char*> FileSystemGUI::getShaderTypeItems()
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

void FileSystemGUI::DrawShadersTable(GUIType::BrowserCache &browser)
{
    auto files = browser.folderFiles;

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("ScriptsFolderTable", 4, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Load", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < files.size(); i++) {
            DrawShaderRow(browser, files[i], i);
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawShaderRow(GUIType::BrowserCache &browser, const std::string &file, int index)
{
    ImGui::PushID(index);
    ImGui::TableNextRow();

    // name
    ImGui::TableSetColumnIndex(0);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
    ImGui::Image(Icon(IconGUI::SHADER_FILE), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    std::string optionText = std::to_string(index + 1) + ") " + file;
    ImGui::Selectable(optionText.c_str());
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        static std::string folderCopy, fileCopy;
        folderCopy = browser.currentFolder;
        fileCopy = file;

        Config::DragDropCustomShaderData data = {folderCopy.c_str(), fileCopy.c_str()};
        ImGui::SetDragDropPayload(GUIType::DragDropTarget::SHADER_ITEM, &data, sizeof(data));

        auto fullPath = browser.currentFolder + file;
        ImGui::Text("%s", fullPath.c_str());
        ImGui::EndDragDropSource();
    }

    DrawShaderRowActions(browser, file);

    ImGui::PopID();
}

void FileSystemGUI::DrawShaderRowActions(GUIType::BrowserCache &browser, const std::string &file)
{
    auto fullPath = browser.currentFolder + file;

    // Load button
    ImGui::TableSetColumnIndex(1);
    GUI::DrawButtonTransparent("Load shader into scene", IconGUI::SHADER_LOAD, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
        Components::get()->Render()->LoadShaderIntoScene(fullPath);
    });

    // Edit button
    ImGui::TableSetColumnIndex(2);
    GUI::DrawButton("Edit shader", IconGUI::SHADER_EDIT, GUIType::Sizes::ICONS_BROWSERS, false,[&] {
        ShadersGUI::LoadDialogShader(fullPath);
    });

    // Delete button
    ImGui::TableSetColumnIndex(3);

    GUI::DrawButtonConfirm("Deleting shader", "Are you sure to delete shader?", IconGUI::SHADER_REMOVE, GUIType::Sizes::ICONS_BROWSERS, [&] {
        ShaderOGLCustom::RemoveCustomShaderFiles(
            browser.currentFolder,
            Tools::getFilenameWithoutExtension(file)
        );
        browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
    });
}

void FileSystemGUI::DrawShaderCreator(int &item_current_idx, const std::vector<const char*> &items)
{

}

void FileSystemGUI::DrawShaderFiles(GUIType::BrowserCache &browser)
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::FILES_SHADERS);
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

    DrawShaderCreatorDialog(browser, title);
    ImGui::Separator();
    DrawBrowserFolders(Config::get()->CUSTOM_SHADERS_FOLDER, browser, Config::get()->SHADERS_EXT);
    ImGui::Separator();
    DrawShadersTable(browser);
}

void FileSystemGUI::DrawShaderCreatorDialog(GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static int item_current_idx = 0;
        auto items = getShaderTypeItems();

        static char localVarName[256] = "";

        ImGui::SetNextItemWidth(150);
        ImGui::InputText("Shader name##", localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);
        DrawShaderTypeCombo(item_current_idx, items);

        ImGui::Separator();

        float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalWidth = buttonWidth * 2 + spacing;

        float availWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalWidth);

        GUI::DrawButton("Cancel create shader", IconGUI::CANCEL, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            localVarName[0] = '\0';
            ImGui::CloseCurrentPopup();
        });
        ImGui::SameLine();
        GUI::DrawButton("Create shader", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            if (localVarName[0] != '\0') {
                auto type = ShaderOGLCustom::getShaderTypeFromString(items[item_current_idx]);
                ShaderOGLCustom::WriteEmptyCustomShaderToDisk(
                    localVarName,
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

void FileSystemGUI::DrawScriptCreator()
{

}

void FileSystemGUI::DrawScriptsTable(GUIType::BrowserCache &browser)
{
    auto files = browser.folderFiles;

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("ScriptsFolderTable", 3, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < files.size(); i++) {
            DrawScriptRow(browser, files[i], i);
        }
        ImGui::EndTable();
    }
}

void FileSystemGUI::DrawScriptRow(GUIType::BrowserCache &browser, const std::string &file, int index)
{
    ImGui::PushID(index);
    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 5.0f, ImGui::GetCursorPosY() + 5.0f));
    ImGui::Image(Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_BROWSERS);

    ImGui::SameLine();
    std::string optionText = std::to_string(index + 1) + ") " + file;
    ImGui::Selectable(optionText.c_str());
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        auto fullPath = browser.currentFolder + file;
        ImGui::SetDragDropPayload(GUIType::DragDropTarget::SCRIPT_ITEM, fullPath.c_str(), fullPath.size() + 1);
        ImGui::Text("%s", fullPath.c_str());
        ImGui::EndDragDropSource();
    }
    DrawScriptRowActions(browser, file);

    ImGui::PopID();
}

void FileSystemGUI::DrawScriptRowActions(GUIType::BrowserCache &browser, const std::string &file)
{
    auto fullPath = browser.currentFolder + file;

    ImGui::TableSetColumnIndex(1);
    GUI::DrawButton("Edit script", IconGUI::SCRIPT_EDIT, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
        ScriptLuaGUI::LoadScriptDialog(fullPath);
    });
    ImGui::TableSetColumnIndex(2);
    GUI::DrawButtonConfirm("Deleting script", "Are you sure to delete script?", IconGUI::SCRIPT_REMOVE, GUIType::Sizes::ICONS_BROWSERS,[&] {
        ComponentScripting::RemoveScriptLUAFile(fullPath);
        browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCRIPTS_EXT);
    });
}

void FileSystemGUI::DrawScriptCreatorDialog(GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        static char localVarName[256] = "";
        ImGui::SetNextItemWidth(150);
        ImGui::InputText("Script name##", localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);

        ImGui::Separator();

        // Calcular ancho de los botones
        float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalWidth = buttonWidth * 2 + spacing;

        // Alinear a la derecha
        float availWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalWidth);

        GUI::DrawButton("Cancel create script", IconGUI::CANCEL, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            localVarName[0] = '\0';
            ImGui::CloseCurrentPopup();
        });
        ImGui::SameLine();
        GUI::DrawButton("Create script", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            if (localVarName[0] != '\0') {
                ComponentScripting::createScriptLUAFile(browser.currentFolder + localVarName);
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCRIPTS_EXT);
                ImGui::CloseCurrentPopup();
            }
        });
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawScriptFiles(GUIType::BrowserCache &browser)
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::FILES_SCRIPTS);
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
    DrawScriptCreatorDialog(browser, title);
    ImGui::Separator();
    DrawBrowserFolders(Config::get()->SCRIPTS_FOLDER, browser, Config::get()->SCRIPTS_EXT);
    ImGui::Separator();
    DrawScriptsTable(browser);
}