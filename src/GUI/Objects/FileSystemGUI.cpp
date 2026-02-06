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
#include "../../../include/GUI/AddOns/CustomTreeNode.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Loaders/ProjectLoader.h"
#include "../../../include/Loaders/SceneChecker.h"
#include "../../../include/Loaders/SceneLoader.h"
#include "../../../include/Misc/Tools.h"
#include "../../../include/Render/Drawable.h"

bool FileSystemGUI::openPopUpCreateProject = false;
bool FileSystemGUI::openPopupCreateScene = false;
bool FileSystemGUI::openPopupCreateShader = false;
bool FileSystemGUI::openPopupCreateScript = false;
bool FileSystemGUI::autoExpandProject = false;
bool FileSystemGUI::autoExpandScene = false;

void FileSystemGUI::DrawMainBrowser()
{

    auto GUI = Brakeza::get()->GUI();
    auto &projectBrowser = GUI->getBrowserProjects();
    auto &scenesBrowser = GUI->getBrowserScenes();
    auto &scriptsBrowser = GUI->getBrowserScripts();
    auto &shadersBrowser = GUI->getBrowserShaders();

    if (ImGui::BeginTable("BrowseTable", 2,
        ImGuiTableFlags_SizingStretchSame |
        ImGuiTableFlags_NoPadOuterX))
    {
        ImGui::TableSetupColumn("Column1", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Column2", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();

        // Columna izquierda: Projects
        ImGui::TableSetColumnIndex(0);
        GUI::DrawButton(
            "Browse Projects",
            IconGUI::PROJECT_FILE,
            GUIType::Sizes::ICONS_BROWSERS,
            type == GUIType::BROWSE_PROJECTS,
            [] {
                type = GUIType::BROWSE_PROJECTS;
            }
        );

        // Columna derecha: Scenes, Scripts, Shaders (alineados a la derecha)
        ImGui::TableSetColumnIndex(1);

        // Guardar la posición inicial de la columna
        float columnStartX = ImGui::GetCursorPosX();

        // Calcular el ancho de cada botón
        ImGuiStyle& style = ImGui::GetStyle();
        float iconWidth = GUIType::Sizes::ICONS_BROWSERS.x;
        float spacing = style.ItemSpacing.x;
        float framePadding = style.FramePadding.x * 2;

        float width1 = iconWidth + framePadding;
        float width2 = iconWidth + framePadding;
        float width3 = iconWidth + framePadding;

        float totalWidth = width1 + width2 + width3 + (spacing * 2);

        // Obtener el ancho de la columna actual
        float columnWidth = ImGui::GetContentRegionAvail().x;

        // Calcular la posición absoluta para alinear a la derecha
        if (totalWidth < columnWidth) {
            ImGui::SetCursorPosX(columnStartX + columnWidth - totalWidth);
        }

        GUI::DrawButton("Browse Scenes", IconGUI::SCENE_FILE, GUIType::Sizes::ICONS_BROWSERS,
            type == GUIType::BROWSE_SCENES, [] { type = GUIType::BROWSE_SCENES; });

        ImGui::SameLine();

        GUI::DrawButton("Browse Scripts", IconGUI::SCRIPT_FILE, GUIType::Sizes::ICONS_BROWSERS,
            type == GUIType::BROWSE_SCRIPTS, [] { type = GUIType::BROWSE_SCRIPTS; });

        ImGui::SameLine();

        GUI::DrawButton("Browse Shaders", IconGUI::SHADER_FILE, GUIType::Sizes::ICONS_BROWSERS,
            type == GUIType::BROWSE_SHADERS, [] { type = GUIType::BROWSE_SHADERS; });

        ImGui::EndTable();
    }
    ImGui::Separator();

    if (type == GUIType::BROWSE_PROJECTS) {
        DrawProjectFiles(projectBrowser);
    }

    if (type == GUIType::BROWSE_SCENES) {
        DrawSceneFiles(scenesBrowser);
    }

    if (type == GUIType::BROWSE_SCRIPTS) {
        DrawScriptFiles(scriptsBrowser);
    }

    if (type == GUIType::BROWSE_SHADERS) {
        DrawShaderFiles(shadersBrowser);
    }
}

void FileSystemGUI::DrawProjectsTable(GUIType::BrowserCache &browser)
{
    std::vector<std::string> files = browser.folderFiles;

    if (files.empty()) {
        ImGui::Spacing();
        Drawable::WarningMessage("Empty directory");
        return;
    }

    std::sort(files.begin(), files.end());

    for (int i = 0; i < files.size(); i++) {
        const auto& file = files[i];
        if (strcmp(file.c_str(), ".") == 0 || strcmp(file.c_str(), "..") == 0) {
            continue;
        }

        auto fullPath = browser.currentFolder + file;

        // Configurar CustomTreeNode
        std::string name = std::to_string(i + 1) + ") " + file;
        CustomImGui::CustomTreeNodeConfig config(name.c_str());

        config.leftIcon = Icon(IconGUI::PROJECT_FILE);
        config.iconSize = GUIType::Sizes::ICONS_BROWSERS;
        config.isLeaf = true;  // No tiene hijos, no se expande
        config.itemPadding = 1.5f;
        config.itemMargin = 6.0f;

        // Information
        CustomImGui::TreeActionItem infoItem(
            Icon(IconGUI::PROJECT_INFO),
            "Project information",
            [fullPath]() {
                Brakeza::get()->GUI()->getProjectChecker().LoadProjectInfoDialog(fullPath);
            }
        );
        infoItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(infoItem);

        // Open
        CustomImGui::TreeActionItem openItem(
            Icon(IconGUI::PROJECT_OPEN),
            "Open Project",
            [fullPath]() {
                ProjectLoader::LoadProject(fullPath);
            }
        );
        openItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(openItem);

        // Save/Override (directo sin confirmación)
        CustomImGui::TreeActionItem saveItem(
            Icon(IconGUI::SAVE),
            "Override Project",
            [fullPath]() {
                ProjectLoader::SaveProject(fullPath);
            }
        );
        saveItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(saveItem);

        // Delete (directo sin confirmación)
        CustomImGui::TreeActionItem removeItem(
            Icon(IconGUI::PROJECT_REMOVE),
            "Delete project",
            [&browser, fullPath]() {
                ProjectLoader::RemoveProject(fullPath);
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
            }
        );
        removeItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(removeItem);

        // Dibujar el nodo
        CustomImGui::CustomTreeNode(config);
    }
}

void FileSystemGUI::DrawProjectFiles(GUIType::BrowserCache &browser)
{
    if (openPopUpCreateProject) {
        ImGui::OpenPopup(Config::get()->MODAL_CREATE_PROJECT_TITLE_ID.c_str());
        openPopUpCreateProject = false;
    }

    DrawProjectCreatorDialog(browser, Config::get()->MODAL_CREATE_PROJECT_TITLE_ID);

    ImGui::Image(Icon(IconGUI::FOLDER_CURRENT), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
    ImGui::SameLine();
    ImGui::Text("%s", Tools::removeSubstring(browser.currentFolder, Config::get()->ASSETS_FOLDER).c_str());

    ImGui::Separator();
    DrawProjectsTable(browser);
    ImGui::Separator();
    GUI::ImageButtonNormal(IconGUI::PROJECT_FILE, "Create Project", [&] {
        openPopUpCreateProject = true;
    });
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

        GUI::ImageButtonNormal(IconGUI::CANCEL, "Cancel", [&] {
            localVarName[0] = '\0';
            ImGui::CloseCurrentPopup();
        });

        float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;
        float spacing = 150.0f - buttonWidth * 2; // Ajusta este valor según necesites

        ImGui::SameLine(0, spacing);
        GUI::ImageButtonNormal(IconGUI::CREATE_FILE, "Create", [&] {
            if (localVarName[0] != '\0') {
                ProjectLoader::CreateProject(browser.currentFolder + localVarName);
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->PROJECTS_EXT);
                ImGui::CloseCurrentPopup();
            }
        });
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawScenesTable(GUIType::BrowserCache &browser)
{
    auto files = browser.folderFiles;

    if (files.empty()) {
        ImGui::Spacing();
        Drawable::WarningMessage("Empty directory");
        return;
    }

    std::sort(files.begin(), files.end());

    for (int i = 0; i < files.size(); i++) {
        const auto& file = files[i];
        if (strcmp(file.c_str(), ".") == 0 || strcmp(file.c_str(), "..") == 0) {
            continue;
        }

        auto fullPath = browser.currentFolder + file;

        // Configurar CustomTreeNode
        std::string name = std::to_string(i + 1) + ") " + file;
        CustomImGui::CustomTreeNodeConfig config(name.c_str());

        config.leftIcon = Icon(IconGUI::SCENE_FILE);
        config.iconSize = GUIType::Sizes::ICONS_BROWSERS;
        config.isLeaf = true;
        config.itemPadding = 1.5f;
        config.itemMargin = 6.0f;


        // Information
        CustomImGui::TreeActionItem infoItem(
            Icon(IconGUI::SCENE_INFO),
            "Scene information",
            [fullPath]() {
                Brakeza::get()->GUI()->getSceneChecker().LoadSceneInfoDialog(fullPath);
            }
        );
        infoItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(infoItem);

        // Load
        CustomImGui::TreeActionItem loadItem(
            Icon(IconGUI::SCENE_LOAD),
            "Load Scene file",
            [fullPath]() {
                SceneLoader::ClearScene();
                SceneLoader::LoadScene(fullPath);
            }
        );
        loadItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(loadItem);

        // Save (directo sin confirmación)
        CustomImGui::TreeActionItem saveItem(
            Icon(IconGUI::SAVE),
            "Override scene",
            [fullPath]() {
                SceneLoader::SaveScene(fullPath);
            }
        );
        saveItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(saveItem);

        // Delete (directo sin confirmación)
        CustomImGui::TreeActionItem removeItem(
            Icon(IconGUI::SCENE_REMOVE),
            "Delete scene",
            [&browser, fullPath]() {
                SceneLoader::RemoveScene(fullPath);
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
            }
        );
        removeItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(removeItem);

        // Dibujar el nodo
        CustomImGui::CustomTreeNode(config);

        // Drag & Drop Source (después de dibujar)
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload(
                GUIType::DragDropTarget::SCENE_ITEM,
                fullPath.c_str(),
                fullPath.size() + 1
            );
            ImGui::Text("%s", fullPath.c_str());
            ImGui::EndDragDropSource();
        }
    }
}

void FileSystemGUI::DrawSceneFiles(GUIType::BrowserCache &browser)
{
    if (openPopupCreateScene) {
        ImGui::OpenPopup(Config::get()->MODAL_CREATE_SCENE_TITLE_ID.c_str());
        openPopupCreateScene = false;
    }
    DrawSceneCreatorDialog(browser, Config::get()->MODAL_CREATE_SCENE_TITLE_ID);

    DrawBrowserFolders(Config::get()->SCENES_FOLDER, browser);
    ImGui::Separator();
    DrawScenesTable(browser);
    ImGui::Separator();
    GUI::ImageButtonNormal(IconGUI::SCENE_FILE, "Create Scene", [&] {
        openPopupCreateScene = true;
    });
}

void FileSystemGUI::DrawSceneCreatorDialog(GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SameLine();
        static char localVarName[256] = "";

        ImGui::SetNextItemWidth(150);
        ImGui::InputText("Scene name##", localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);
        ImGui::Separator();

        GUI::ImageButtonNormal(IconGUI::CANCEL, "Cancel", [&] {
            localVarName[0] = '\0';
            ImGui::CloseCurrentPopup();
        });

        float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;
        float spacing = 150.0f - buttonWidth * 2;

        ImGui::SameLine(0, spacing);
        GUI::ImageButtonNormal(IconGUI::CREATE_FILE, "Create", [&] {
            if (localVarName[0] != '\0') {
                SceneLoader::CreateScene(browser.currentFolder + localVarName);
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCENES_EXT);
                ImGui::CloseCurrentPopup();
            }
        });
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawBrowserFolders(const std::string& rootFolder, GUIType::BrowserCache &browser)
{
    ImGui::Image(Icon(IconGUI::FOLDER_CURRENT), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
    ImGui::SameLine();
    ImGui::Text("%s", Tools::removeSubstring(browser.currentFolder, Config::get()->ASSETS_FOLDER).c_str());
    ImGui::Separator();

    ImVec2 iconSize = GUIType::Sizes::ICONS_BROWSERS;
    float frameHeight = ImGui::GetFrameHeight();
    float offsetY = (frameHeight - iconSize.y) * 0.5f;

    // only show ".." if we arent in browser's root folder
    if (browser.currentFolder != rootFolder) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);  // Bajar cursor
        ImGui::Image(Icon(IconGUI::FOLDER_BACK), iconSize);
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);  // Subir cursor

        if (ImGui::Button("..")) {
            browser.currentFolder = Tools::NormalizePath(Tools::GoBackFromFolder(browser.currentFolder));
            browser.onChangeFolderCallback();
        }
    }

    if (browser.folderFolders.empty()) return;

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
        LOG_MESSAGE("Selected folder %s", selectedFolder.c_str());
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

void FileSystemGUI::DrawWinMediaBrowser(GUIType::BrowserCache &browser, TexturePackage &package)
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::MEDIA_BROWSER);
    if (!windowStatus->isOpen) return;

    // Selector Images/Textures
    static int selectedFolderIndex = 0;
    const char* folderOptions[] = { "Images", "Textures" };
    std::string rootFolders[] = { Config::get()->IMAGES_FOLDER, Config::get()->TEXTURES_FOLDER };

    ImGui::SetNextItemWidth(120);
    if (ImGui::Combo("Source##images_folder", &selectedFolderIndex, folderOptions, IM_ARRAYSIZE(folderOptions))) {
        browser.currentFolder = rootFolders[selectedFolderIndex];
        browser.onChangeFolderCallback();
    }
    static float imageSize = 96.0f;
    // Slider
    ImGui::SameLine();
    ImGui::PushItemWidth(200);
    ImGui::SliderFloat("Size", &imageSize, 64.0f, 256.0f, "%.0f px");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Reset")) imageSize = 96.0f;

    ImGui::Separator();

    DrawBrowserFolders(rootFolders[selectedFolderIndex], browser);
    if (browser.folderFolders.empty()) {
        ImGui::Spacing();
        Drawable::WarningMessage("There are not subfolders");
    }

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
                ImGui::SetDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM, image->srcPath.c_str(), image->srcPath.size() + 1);
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

    if (files.empty()) {
        ImGui::Spacing();
        Drawable::WarningMessage("Empty directory");
        return;
    }

    for (int i = 0; i < files.size(); i++) {
        auto file = files[i];
        auto fullPath = browser.currentFolder + file;

        // Configurar CustomTreeNode
        std::string name = std::to_string(i + 1) + ") " + file;
        CustomImGui::CustomTreeNodeConfig config(name.c_str());

        config.leftIcon = Icon(IconGUI::SHADER_FILE);
        config.iconSize = GUIType::Sizes::ICONS_BROWSERS;
        config.isLeaf = true;  // No tiene hijos, no se expande
        config.itemPadding = 1.5f;
        config.itemMargin = 6.0f;


        // Load
        CustomImGui::TreeActionItem loadItem(
            Icon(IconGUI::SHADER_LOAD),
            "Attach shader to current scene",
            [fullPath]() {
                Components::get()->Render()->LoadShaderIntoScene(fullPath);
                FileSystemGUI::autoExpandScene = true;
            }
        );
        loadItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(loadItem);

        // Edit
        CustomImGui::TreeActionItem editItem(
            Icon(IconGUI::SHADER_EDIT),
            "Edit Shader",
            [fullPath]() {
                ShadersGUI::LoadDialogShader(fullPath);
            }
        );
        editItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(editItem);

        // Delete (directo sin confirmación)
        CustomImGui::TreeActionItem removeItem(
            Icon(IconGUI::SHADER_REMOVE),
            "Delete shader",
            [&browser, file]() {
                ShaderBaseCustomOGLCode::RemoveCustomShaderFiles(
                    browser.currentFolder,
                    Tools::getFilenameWithoutExtension(file)
                );
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SHADERS_EXT);
            }
        );
        removeItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(removeItem);

        // Dibujar el nodo
        CustomImGui::CustomTreeNode(config);

        // Drag & Drop Source (después de dibujar) - Usa estructura específica para shaders
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            static std::string folderCopy, fileCopy;
            folderCopy = browser.currentFolder;
            fileCopy = file;

            Config::DragDropCustomShaderData data = {folderCopy.c_str(), fileCopy.c_str()};
            ImGui::SetDragDropPayload(GUIType::DragDropTarget::SHADER_ITEM, &data, sizeof(data));

            ImGui::Text("%s", fullPath.c_str());
            ImGui::EndDragDropSource();
        }
    }
}

void FileSystemGUI::DrawShaderFiles(GUIType::BrowserCache &browser)
{
    if (openPopupCreateShader) {
        ImGui::OpenPopup(Config::get()->MODAL_CREATE_SHADER_TITLE_ID.c_str());
        openPopupCreateShader = false;
    }

    DrawShaderCreatorDialog(browser, Config::get()->MODAL_CREATE_SHADER_TITLE_ID);

    DrawBrowserFolders(Config::get()->CUSTOM_SHADERS_FOLDER, browser);
    ImGui::Separator();
    DrawShadersTable(browser);
    ImGui::Separator();
    GUI::ImageButtonNormal(IconGUI::SHADER_FILE, "Create shader", [&] {
        openPopupCreateShader = true;
    });
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

        GUI::ImageButtonNormal(IconGUI::CANCEL, "Cancel", [&] {
            localVarName[0] = '\0';
            ImGui::CloseCurrentPopup();
        });

        float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;
        float spacing = 150.0f - buttonWidth * 2; // Ajusta este valor según necesites

        ImGui::SameLine(0, spacing);
        GUI::ImageButtonNormal(IconGUI::CREATE_FILE, "Create", [&] {
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

    if (files.empty()) {
        ImGui::Spacing();
        Drawable::WarningMessage("Empty directory");
        return;
    }

    for (int i = 0; i < files.size(); i++) {
        auto file = files[i];
        auto fullPath = browser.currentFolder + file;

        std::string name = std::to_string(i + 1) + ") " + file;
        CustomImGui::CustomTreeNodeConfig config(name.c_str());

        config.leftIcon = Icon(IconGUI::SCRIPT_FILE);
        config.iconSize = GUIType::Sizes::ICONS_BROWSERS;
        config.isLeaf = true;
        config.itemPadding = 1.5f;
        config.itemMargin = 6.0f;

        // Load
        CustomImGui::TreeActionItem loadItem(
            Icon(IconGUI::SCRIPT_LOAD),
            "Attach script to current scene",
            [fullPath]() {
                auto meta = ScriptLuaGUI::ExtractScriptMetainfo(fullPath);
                auto script = new ScriptLUA(meta.name, meta.codeFile, meta.typesFile);
                if (script->getType() != SCRIPT_GLOBAL) {
                    LOG_ERROR("[Scene] Error: Cannot attach Object script to Scene. Only Global scripts are allowed.");
                    delete script;
                    return;
                }
                Components::get()->Scripting()->AddSceneLUAScript(script);
                FileSystemGUI::autoExpandScene = true;
            }
        );
        loadItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(loadItem);

        // Edit
        CustomImGui::TreeActionItem editItem(
            Icon(IconGUI::SCRIPT_EDIT),
            "Edit script",
            [fullPath]() {
                ScriptLuaGUI::LoadScriptDialog(fullPath);
            }
        );
        editItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(editItem);

        // Remove (directo sin confirmación)
        CustomImGui::TreeActionItem removeItem(
            Icon(IconGUI::SCRIPT_REMOVE),
            "Delete script",
            [&browser, fullPath]() {
                ComponentScripting::RemoveScriptLUAFile(fullPath);
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCRIPTS_EXT);
            }
        );
        removeItem.size = GUIType::Sizes::ICONS_BROWSERS;
        config.actionItems.push_back(removeItem);

        CustomImGui::CustomTreeNode(config);

        // Drag & Drop Source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload(GUIType::DragDropTarget::SCRIPT_ITEM, fullPath.c_str(), fullPath.size() + 1);
            ImGui::Text("%s", fullPath.c_str());
            ImGui::EndDragDropSource();
        }
    }
}

void FileSystemGUI::DrawScriptCreatorDialog(GUIType::BrowserCache &browser, std::string &title)
{
    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

        static char localVarName[256] = "";
        static int scriptTypeIndex = 0;
        const char* scriptTypeItems[] = { "Global", "Object" };

        ImGui::SetNextItemWidth(150);
        ImGui::InputText("Script name##", localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);

        ImGui::SetNextItemWidth(150);
        ImGui::Combo("Type##script_type", &scriptTypeIndex, scriptTypeItems, IM_ARRAYSIZE(scriptTypeItems));

        ImGui::Separator();

        GUI::ImageButtonNormal(IconGUI::CANCEL, "Cancel", [&] {
            localVarName[0] = '\0';
            ImGui::CloseCurrentPopup();
        });
        float buttonWidth = GUIType::Sizes::ICONS_BROWSERS.x + ImGui::GetStyle().FramePadding.x * 2;
        float spacing = 150.0f - buttonWidth * 2;

        ImGui::SameLine(0, spacing);
        GUI::ImageButtonNormal(IconGUI::CREATE_FILE, "Create", [&] {
            if (localVarName[0] != '\0') {
                ScriptType selectedType = (scriptTypeIndex == 0) ? SCRIPT_GLOBAL : SCRIPT_OBJECT;
                ComponentScripting::CreateScriptLUAFile(browser.currentFolder + localVarName, selectedType);
                browser.folderFiles = Tools::getFolderFiles(browser.currentFolder, Config::get()->SCRIPTS_EXT);
                ImGui::CloseCurrentPopup();
            }
        });
        ImGui::EndPopup();
    }
}

void FileSystemGUI::DrawScriptFiles(GUIType::BrowserCache &browser)
{
    if (openPopupCreateScript) {
        ImGui::OpenPopup(Config::get()->MODAL_CREATE_SCRIPT_TITLE_ID.c_str());
        openPopupCreateScript = false;
    }

    DrawScriptCreatorDialog(browser, Config::get()->MODAL_CREATE_SCRIPT_TITLE_ID);
    DrawBrowserFolders(Config::get()->SCRIPTS_FOLDER, browser);

    ImGui::Separator();
    DrawScriptsTable(browser);
    ImGui::Separator();
    GUI::ImageButtonNormal(IconGUI::SCRIPT_FILE, "Create script", [&] {
        openPopupCreateScript = true;
    });
}