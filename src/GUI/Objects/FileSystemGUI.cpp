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
#include "../../../include/Loaders/SceneChecker.h"
#include "../../../include/Loaders/SceneLoader.h"
#include "../../../include/Misc/Tools.h"
#include "../../../include/Render/Drawable.h"

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

    GUI::DrawButtonTransparent("Project information", IconGUI::PROJECT_INFO, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
        Brakeza::get()->GUI()->getProjectChecker().LoadProjectInfoDialog(fullPath);
    });
    ImGui::SameLine();
    GUI::DrawButtonTransparent("Open project", IconGUI::PROJECT_OPEN, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
        ProjectLoader::LoadProject(fullPath);
    });
    ImGui::SameLine();
    GUI::DrawButtonConfirm("Override project", "Are you sure to override project?", IconGUI::SAVE, GUIType::Sizes::ICONS_BROWSERS, [&] {
        ProjectLoader::SaveProject(fullPath);
    });
    ImGui::SameLine();
    GUI::DrawButtonConfirm("Deleting project", "Are you sure to delete project?", IconGUI::PROJECT_REMOVE, GUIType::Sizes::ICONS_BROWSERS, [&] {
        ProjectLoader::RemoveProject(fullPath);
        browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
    });
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

void FileSystemGUI::DrawScenesTable(GUIType::BrowserCache &browser)
{
    auto files = browser.folderFiles;
    std::sort(files.begin(), files.end());

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("ScenesFolderTable", 5, flags)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Information", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Load", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthFixed);
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

    std::string optionText = std::to_string(index + 1) + ") " + file;
    ImGui::Selectable(optionText.c_str());
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        auto fullPath = browser.currentFolder + file;

        ImGui::SetDragDropPayload(
            GUIType::DragDropTarget::SCENE_ITEM,
            fullPath.c_str(),
            fullPath.size() + 1
        );

        ImGui::Text("%s", fullPath.c_str());
        ImGui::EndDragDropSource();
    }

    ImGui::TableSetColumnIndex(1);
    DrawSceneRowActions(browser, file);

    ImGui::PopID();
}

void FileSystemGUI::DrawSceneRowActions(GUIType::BrowserCache &browser, const std::string &file)
{
    auto fullPath = browser.currentFolder + file;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));

    GUI::DrawButtonTransparent("Scene information", IconGUI::SCENE_INFO, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
        Brakeza::get()->GUI()->getSceneChecker().LoadSceneInfoDialog(fullPath);
    });
    ImGui::TableSetColumnIndex(2);
    // Load button
    GUI::DrawButtonTransparent("Load scene", IconGUI::SCENE_LOAD, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
        SceneLoader::ClearScene();
        SceneLoader::LoadScene(fullPath);
    });
    ImGui::TableSetColumnIndex(3);
    // Save button
    GUI::DrawButtonConfirm("Overriding scene", "Are you sure to override scene?", IconGUI::SAVE, GUIType::Sizes::ICONS_BROWSERS, [&] {
        SceneLoader::SaveScene(fullPath);
    });
    ImGui::TableSetColumnIndex(4);
    // Delete button
    GUI::DrawButtonConfirm("Deleting scene", "Are you sure to delete scene?", IconGUI::SCENE_REMOVE, GUIType::Sizes::ICONS_BROWSERS, [&] {
        SceneLoader::RemoveScene(fullPath);
        browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
    });

    ImGui::PopStyleVar();
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
    DrawBrowserFolders(Config::get()->SCENES_FOLDER, browser);
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

void FileSystemGUI::DrawBrowserFolders(const std::string& rootFolder, GUIType::BrowserCache &browser)
{
    // only show ".." if we arent in browser's root folder
    if (browser.currentFolder != rootFolder) {
        ImGui::AlignTextToFramePadding();
        ImGui::Image(Icon(IconGUI::FOLDER), ImVec2(24, 24));
        ImGui::SameLine();
        if (ImGui::Button("..")) {
            browser.currentFolder = Tools::NormalizePath(Tools::GoBackFromFolder(browser.currentFolder));
            browser.onChangeFolderCallback();
            return;
        }
    }

    if (browser.folderFolders.empty()) return;

    // Para el icono de la carpeta actual
    ImVec2 iconSize = GUIType::Sizes::ICONS_BROWSERS;
    float frameHeight = ImGui::GetFrameHeight();
    float offsetY = (frameHeight - iconSize.y) * 0.5f;

    // IMPORTANTE: No modificar el vector durante la iteración
    std::string selectedFolder;

    for (const auto &i : browser.folderFolders) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
        ImGui::Image(Icon(IconGUI::FOLDER), iconSize);
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);

        if (ImGui::Button(i.c_str())) {
            selectedFolder = Tools::NormalizePath(browser.currentFolder + i);
            break;
        }
    }

    // Cambiar de carpeta DESPUÉS de terminar la iteración
    if (!selectedFolder.empty()) {
        LOG_MESSAGE(selectedFolder.c_str());
        browser.currentFolder = selectedFolder;
        browser.onChangeFolderCallback();
    }
}

void FileSystemGUI::LoadImagesFolder(GUIType::BrowserCache &browser, TexturePackage &package)
{
    package.Clear();

    auto images = Tools::getFolderFiles(browser.currentFolder, Config::get()->IMAGES_EXT);

    for (const auto& f: images) {
        package.addItem(browser.currentFolder + f, f);
    }
}

ImTextureID FileSystemGUI::Icon(GUIType::Sheet coords)
{
    return Brakeza::get()->GUI()->getTextureAtlas()->getTextureByXY(coords.x, coords.y)->getOGLImTexture();
}

void FileSystemGUI::DrawWinImages(GUIType::BrowserCache &browser, TexturePackage &package)
{
    ImGui::Image(Icon(IconGUI::IMAGE_FILE), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::Text("Current Folder: %s", browser.currentFolder.c_str());
    ImGui::Separator();

    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::IMAGES);
    if (!windowStatus->isOpen) return;

    DrawBrowserFolders(Config::get()->IMAGES_FOLDER, browser);
    ImGui::Separator();

    // Slider
    static float imageSize = 96.0f;
    ImGui::PushItemWidth(200);
    ImGui::SliderFloat("Size", &imageSize, 64.0f, 256.0f, "%.0f px");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Reset")) imageSize = 96.0f;

    ImGui::Separator();

    if (package.getItems().empty()) {
        Drawable::WarningMessage("No images found");
        return;
    }

    // Calcular columnas dinámicamente según el ancho disponible
    float availableWidth = ImGui::GetContentRegionAvail().x;
    float cellWidth = imageSize + 20.0f; // Tamaño de imagen + padding
    int columns = std::max(1, (int)(availableWidth / cellWidth));

    // Usar padding en las celdas
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoPadOuterX;

    if (ImGui::BeginTable("ImagesTable", columns, flags)) {
        int count = 0;
        for (auto &image : package.getItems()) {
            if (!image->texture->isLoaded()) continue;

            if (count % columns == 0) {
                ImGui::TableNextRow();
            }
            ImGui::TableNextColumn();

            // Centrar todo el grupo
            float columnWidth = ImGui::GetContentRegionAvail().x;
            float offsetX = (columnWidth - imageSize) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
            ImGui::BeginGroup();

            // Imagen con botón
            ImGui::PushID(image->label.c_str());

            // Calcular aspect ratio para mantener proporciones
            float imgWidth = (float)image->texture->width();
            float imgHeight = (float)image->texture->height();
            float aspectRatio = imgWidth / imgHeight;

            ImVec2 displaySize;
            if (aspectRatio > 1.0f) {
                // Imagen horizontal
                displaySize = ImVec2(imageSize, imageSize / aspectRatio);
            } else {
                // Imagen vertical o cuadrada
                displaySize = ImVec2(imageSize * aspectRatio, imageSize);
            }

            ImGui::ImageButton(
                reinterpret_cast<ImTextureID>(image->texture->getOGLTextureID()),
                displaySize,
                ImVec2(0, 0), ImVec2(1, 1),
                2 // Frame padding
            );

            // Drag & Drop
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM, image->label.c_str(), image->label.size() + 1);
                ImGui::Image(reinterpret_cast<ImTextureID>(image->texture->getOGLTextureID()), ImVec2(48, 48));
                ImGui::SameLine();
                ImGui::Text("%s", image->label.c_str());
                ImGui::EndDragDropSource();
            }

            // Tooltip al pasar el ratón
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Image(reinterpret_cast<ImTextureID>(image->texture->getOGLTextureID()), ImVec2(256, 256));
                ImGui::Text("%s", image->label.c_str());
                ImGui::Text("%d x %d px", image->texture->width(), image->texture->height());
                ImGui::EndTooltip();
            }

            ImGui::PopID();

            // Nombre centrado (truncar si es muy largo)
            std::string displayName = image->label;
            float maxTextWidth = imageSize;
            ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());

            if (textSize.x > maxTextWidth) {
                // Truncar el texto
                while (textSize.x > maxTextWidth - 20 && displayName.length() > 3) {
                    displayName = displayName.substr(0, displayName.length() - 1);
                    textSize = ImGui::CalcTextSize((displayName + "...").c_str());
                }
                displayName += "...";
                textSize = ImGui::CalcTextSize(displayName.c_str());
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (imageSize - textSize.x) * 0.5f);
            ImGui::TextWrapped("%s", displayName.c_str());

            // Dimensiones centradas (más pequeñas)
            char sizeText[32];
            snprintf(sizeText, sizeof(sizeText), "%d×%d", image->texture->width(), image->texture->height());
            ImVec2 sizeTextSize = ImGui::CalcTextSize(sizeText);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (imageSize - sizeTextSize.x) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
            ImGui::Text("%s", sizeText);
            ImGui::PopStyleColor();

            ImGui::EndGroup();

            count++;
        }

        ImGui::EndTable();
    }

    ImGui::PopStyleVar(); // CellPadding
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
        ShaderBaseCustomOGLCode::RemoveCustomShaderFiles(
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
        GUI::DrawButton("Create shader", IconGUI::CREATE_FILE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
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
    DrawBrowserFolders(Config::get()->CUSTOM_SHADERS_FOLDER, browser);
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
                auto type = ShaderBaseCustom::getShaderTypeFromString(items[item_current_idx]);
                ShadersGUI::CreateShaderBaseCustom(type, browser.currentFolder, localVarName);
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
                ImGui::CloseCurrentPopup();
            }
        });
        ImGui::EndPopup();
    }
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
                ComponentScripting::CreateScriptLUAFile(browser.currentFolder + localVarName);
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
    DrawBrowserFolders(Config::get()->SCRIPTS_FOLDER, browser);
    ImGui::Separator();
    DrawScriptsTable(browser);
}