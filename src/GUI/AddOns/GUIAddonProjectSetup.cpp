//
// Created by Eduardo on 06/12/2025.
//

#include "../include/Brakeza.h"
#include "../../../include/Components/Components.h"
#include "../../../include/GUI/AddOns/GUIAddonProjectSetup.h"

#include "../../../include/GUI/AddOns/CustomTreeNode.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/Render/Drawable.h"


void GUIAddonProjectSetup::DrawWinProjectSettings()
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::PROJECT_SETTINGS);
    if (!windowStatus->isOpen) return;

    SaveCurrentProjectButton();
    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 4.0f));
    TreeProjectSettings();
    TreeProjectScripts();
    TreeProjectScenes();
    ImGui::Separator();
    SaveCurrentSceneButton();
    TreeSceneScripts();
    TreeSceneShaders();
    ImGui::Separator();
    ImGui::PopStyleVar();

    ExampleCustomTreeNodeUsage();
}

void GUIAddonProjectSetup::TreeSceneScripts()
{
    auto scripting = Components::get()->Scripting();
    static bool shouldOpen = false;

    std::string labelSceneScripts = "Scene scripts (" + std::to_string(scripting->getSceneScripts().size()) + ")";
    ImGui::Image(FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENE_SCRIPTS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    if (shouldOpen) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        shouldOpen = false;
    } else {
        ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
    }
    bool isOpenSceneScripts = ImGui::TreeNodeEx(labelSceneScripts.c_str(), ImGuiTreeNodeFlags_FramePadding);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::SCRIPT_ITEM)) {
            LOG_MESSAGE("Dropping script (%s) in global space", payload->Data);
            auto meta = ScriptLuaGUI::ExtractScriptMetainfo(std::string((char *) payload->Data));
            scripting->AddSceneLUAScript(new ScriptLUA(meta.name, meta.codeFile, meta.typesFile));
            shouldOpen = true;
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopStyleVar(2);
    if (isOpenSceneScripts) {
        DrawSceneScripts();
        if (scripting->getSceneScripts().empty()) {
            Drawable::WarningMessage("There are not scenes attached");
        }
        ImGui::TreePop();
    }
}

void GUIAddonProjectSetup::TreeProjectScenes()
{
    auto scripting = Components::get()->Scripting();
    static bool shouldOpen = false;

    auto projectScenes = scripting->getProjectScenes();
    std::string labelGlobalScripts = "Project scenes (" + std::to_string(projectScenes.size()) + ")";
    ImGui::Image(FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENES), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (shouldOpen) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        shouldOpen = false;
    } else {
        ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
    }
    bool isOpenGlobalScripts = ImGui::TreeNodeEx(labelGlobalScripts.c_str(), ImGuiTreeNodeFlags_FramePadding);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::SCENE_ITEM)) {
            const char* scenePath = (const char*)payload->Data;
            LOG_MESSAGE("Dropping scene '%s' in global space", scenePath);
            scripting->AddProjectScene(scenePath);
            shouldOpen = true;
        }
        ImGui::EndDragDropTarget();
    }
    if (isOpenGlobalScripts) {
        DrawProjectScenes();
        if (projectScenes.empty()) {
            Drawable::WarningMessage("There are not scenes attached");
        }
        ImGui::TreePop();
    }
}

void GUIAddonProjectSetup::TreeProjectSettings()
{
    static bool shouldOpen = false;

    std::string labelGlobalScripts = "Project settings";
    ImGui::Image(FileSystemGUI::Icon(IconGUI::WIN_PROJECT_SETTINGS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    if (shouldOpen) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        shouldOpen = false;
    } else {
        ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
    }
    bool isOpen = ImGui::TreeNodeEx(labelGlobalScripts.c_str(), ImGuiTreeNodeFlags_FramePadding);

    ImGui::PopStyleVar(2);
    if (isOpen) {
        DrawProjectSettings();
        ImGui::TreePop();
    }
}

void GUIAddonProjectSetup::TreeProjectScripts()
{
    auto scripting = Components::get()->Scripting();
    static bool shouldOpen = false;

    std::string labelGlobalScripts = "Project scripts (" + std::to_string(scripting->getProjectScripts().size()) + ")";
    ImGui::Image(FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_GLOBAL_SCRIPTS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    if (shouldOpen) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        shouldOpen = false;
    } else {
        ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
    }
    bool isOpenGlobalScripts = ImGui::TreeNodeEx(labelGlobalScripts.c_str(), ImGuiTreeNodeFlags_FramePadding);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::SCRIPT_ITEM)) {
            LOG_MESSAGE("Dropping script '%s' in global space", payload->Data);
            auto meta = ScriptLuaGUI::ExtractScriptMetainfo(std::string((char *) payload->Data));
            scripting->AddProjectLUAScript(new ScriptLUA(meta.name, meta.codeFile, meta.typesFile));
            shouldOpen = true;
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopStyleVar(2);
    if (isOpenGlobalScripts) {
        DrawProjectScripts();
        if (scripting->getProjectScripts().empty()) {
            Drawable::WarningMessage("There are not scripts attached");
        }
        ImGui::TreePop();
    }
}

void GUIAddonProjectSetup::TreeSceneShaders()
{
    auto render = Components::get()->Render();
    static bool shouldOpen = false;

    std::string labelSceneShaders = "Scene shaders (" + std::to_string(render->getSceneShaders().size()) + ")";
    ImGui::Image(FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENE_SHADERS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    if (shouldOpen) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        shouldOpen = false;
    } else {
        ImGui::SetNextItemOpen(false, ImGuiCond_FirstUseEver);
    }
    bool isOpenSceneShaders = ImGui::TreeNodeEx(labelSceneShaders.c_str(), ImGuiTreeNodeFlags_FramePadding);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::SHADER_ITEM)) {
            ImGui::SetNextItemOpen(true, ImGuiCond_Always);
            auto receivedData = (Config::DragDropCustomShaderData*) payload->Data;
            auto fullPath = std::string(receivedData->folder) + receivedData->file;
            LOG_MESSAGE("Dropping shader file '%s' in global space...", fullPath.c_str());
            render->LoadShaderIntoScene(fullPath);
            shouldOpen = true;
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopStyleVar(2);
    if (isOpenSceneShaders) {
        DrawSceneCustomShaders();
        if (render->getSceneShaders().empty()) {
            Drawable::WarningMessage("There are not shaders attached");
        }
        ImGui::TreePop();
    }
}

void GUIAddonProjectSetup::SaveCurrentSceneButton()
{
    auto scripting = Components::get()->Scripting();
    if (scripting->getCurrentScene() != nullptr) {
        GUI::ImageButtonNormal(IconGUI::SCENE_SAVE, "Save loaded Scene", [scripting] {
            SceneLoader::SaveScene(scripting->getCurrentScene()->getFilePath());
        });
    }
}

void GUIAddonProjectSetup::SaveCurrentProjectButton()
{
    auto scripting = Components::get()->Scripting();
    if (scripting->getCurrentProject() != nullptr) {
        GUI::ImageButtonNormal(IconGUI::PROJECT_SAVE, "Save loaded Project", [scripting] {
            ProjectLoader::SaveProject(scripting->getCurrentProject()->getFilePath());
        });
    }
}

void GUIAddonProjectSetup::DrawProjectScenes()
{
    auto scripting = Components::get()->Scripting();

    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    auto scenes = scripting->getProjectScenes();

    if (ImGui::BeginTable("ProjectScenesTable", 2, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Scene");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        for (unsigned int i = 0; i < scenes.size(); i++) {
            auto currentScene = scenes[i];
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine(0, 5.0f);
            ImGui::Text(currentScene.c_str());

            // Script details
            ImGui::TableNextColumn();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));    // Padding interno del botón
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));     // Espacio entre botones

            GUI::DrawButtonTransparent("Load scene", IconGUI::SCENE_LOAD, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                SceneLoader::ClearScene();
                SceneLoader::LoadScene(currentScene);
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Remove scene from project", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                Components::get()->Scripting()->RemoveProjectScene(currentScene);
            });

            ImGui::PopStyleVar(2);  // ¡ESTO FALTABA!

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}
void GUIAddonProjectSetup::DrawProjectSettings()
{
    auto scripting = Components::get()->Scripting();
    auto project = scripting->getCurrentProject();

    if (project == nullptr) {
        ImGui::Spacing();
        Drawable::WarningMessage("There are not a project loaded");
        return;
    }

    auto checker = scripting->getCurrentProject()->getChecker();
    if (!checker.isLoaded()) return;

    std::string projectName = checker.getStatus().name;
    std::string filePath = checker.getFilePath();
    Vertex3D gravity = checker.getStatus().gravity;

    int resolutionW = checker.getStatus().resolution.width;
    int resolutionH = checker.getStatus().resolution.height;;

    if (ImGui::BeginTable("ProjectInfoTable", 2)) {
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        // Project Name
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::BULLET), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Project Name:");
        ImGui::TableSetColumnIndex(1);
        ImGui::TextWrapped("%s", projectName.c_str());

        // File Path
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::BULLET), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "File Path:");
        ImGui::TableSetColumnIndex(1);
        ImGui::TextWrapped("%s", filePath.c_str());

        // Gravity
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::BULLET), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Gravity:");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%.2f, %.2f, %.2f", gravity.x, gravity.y, gravity.z);

        // Resolution
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::BULLET), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Resolution:");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d x %d", resolutionW, resolutionH);
        ImGui::EndTable();
    }
}

void GUIAddonProjectSetup::DrawProjectScripts()
{
    auto scripting = Components::get()->Scripting();

    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    auto scripts = scripting->getProjectScripts();

    if (ImGui::BeginTable("ProjectScriptsTable", 2, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Script");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        for (unsigned int i = 0; i < scripts.size(); i++) {
            auto currentScript = scripts[i];
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine(0, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
            bool isOpenCurrentScript = ImGui::TreeNodeEx(currentScript->getName().c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding);
            ImGui::PopStyleVar(2);
            if (isOpenCurrentScript) {
                currentScript->DrawImGuiProperties();
                ImGui::TreePop();
            }

            // Script details
            ImGui::TableNextColumn();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));    // Padding interno del botón
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));     // Espacio entre botones

            GUI::DrawButtonTransparent(
                currentScript->isPaused() ? "Unlock project script##" : "Lock project script##",
                currentScript->isPaused() ? IconGUI::LUA_LOCK : IconGUI::LUA_UNLOCK,
                GUIType::Sizes::ICONS_BROWSERS,
                false,
                [&] { currentScript->setPaused(!currentScript->isPaused());
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Reload scene script", IconGUI::SCRIPT_RELOAD, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                currentScript->Reload();
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Remove script", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                scripting->RemoveProjectScript(currentScript);
            });

            ImGui::PopStyleVar(2);

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUIAddonProjectSetup::DrawSceneScripts()
{
    auto scripting = Components::get()->Scripting();

    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    auto scripts = scripting->getSceneScripts();

    if (ImGui::BeginTable("ScriptsTable", 2, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Script");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);

        for (unsigned int i = 0; i < scripts.size(); i++) {
            auto currentScript = scripts[i];
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine(0, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
            bool isOpenCurrentScript = ImGui::TreeNodeEx(currentScript->getName().c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding);
            ImGui::PopStyleVar(2);
            if (isOpenCurrentScript) {
                currentScript->DrawImGuiProperties();
                ImGui::TreePop();
            }

            // Script details
            ImGui::TableNextColumn();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));    // Padding interno del botón
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));     // Espacio entre botones

            GUI::DrawButtonTransparent(
                currentScript->isPaused() ? "Unlock scene script" : "Lock scene script",
                currentScript->isPaused() ? IconGUI::LUA_LOCK : IconGUI::LUA_UNLOCK,
                GUIType::Sizes::ICONS_BROWSERS,
                false,
                [&] { currentScript->setPaused(!currentScript->isPaused()); }
            );
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Edit script", IconGUI::SCRIPT_EDIT, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                ScriptLuaGUI::LoadScriptDialog(currentScript->getTypesFile());
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Reload scene script", IconGUI::SCRIPT_RELOAD, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                currentScript->Reload();
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Remove scene script", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                scripting->RemoveSceneScript(currentScript);
            });

            ImGui::PopStyleVar(2);

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUIAddonProjectSetup::DrawSceneCustomShaders()
{
    auto render = Components::get()->Render();

    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    auto shaders = render->getSceneShaders();

    if (ImGui::BeginTable("SceneShadersTable", 2, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Shader");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);

        for (int i = 0; i < shaders.size(); i++) {
            auto s = shaders[i];
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine(0, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
            bool isOpenCurrentShader = ImGui::TreeNodeEx(s->getLabel().c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding);
            ImGui::PopStyleVar(2);
            if (isOpenCurrentShader) {
                s->DrawImGuiProperties(nullptr, nullptr);
                ImGui::TreePop();
            }

            // Buttons
            ImGui::TableNextColumn();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));    // padding inner button
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));     // spacing between buttonms

            GUI::DrawButtonTransparent(
                !s->isEnabled() ? "UnLock scene shader" : "Lock scene shader",
                !s->isEnabled() ? IconGUI::SHADER_LOCK : IconGUI::SHADER_UNLOCK,
                GUIType::Sizes::ICON_LOCKS,
                false,
                [&] { s->setEnabled(!s->isEnabled());}
            );
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Edit shader", IconGUI::SHADER_EDIT, GUIType::Sizes::ICON_LOCKS, false, [&] {
                auto jsonFilename = s->getTypesFile();
                ShadersGUI::LoadDialogShader(s->getTypesFile());
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Reload shader", IconGUI::LUA_RELOAD, GUIType::Sizes::ICON_LOCKS, false, [&] {
                s->Reload();
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Remove shader from scene", IconGUI::LUA_REMOVE, GUIType::Sizes::ICON_LOCKS, false, [&] {
                render->RemoveSceneShaderByIndex((int) i);
            });

            ImGui::PopStyleVar(2);

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUIAddonProjectSetup::ExampleCustomTreeNodeUsage()
{
    ImGui::Begin("CustomTreeNode - BOTONES FUNCIONAN");

    // Estructura de datos
    struct TreeItem {
        std::string name;
        bool isFolder;
        int id;
        std::vector<TreeItem> children;
        bool selected = false;

        TreeItem(const std::string& n, bool folder, int i)
            : name(n), isFolder(folder), id(i) {}
    };

    // Estado estático
    static bool initialized = false;
    static TreeItem root("Proyecto Raíz", true, 0);
    static TreeItem* selectedItem = nullptr;
    static int totalClicks = 0;  // 🔢 CONTADOR DE CLICKS

    // Inicializar datos de ejemplo
    if (!initialized) {
        auto& scripts = root.children.emplace_back("Scripts", true, 1);
        scripts.children.emplace_back("PlayerController.cs", false, 2);
        scripts.children.emplace_back("EnemyAI.cs", false, 3);
        scripts.children.emplace_back("GameManager.cs", false, 4);

        auto& scenes = root.children.emplace_back("Scenes", true, 5);
        scenes.children.emplace_back("MainMenu.scene", false, 6);
        scenes.children.emplace_back("Level01.scene", false, 7);

        auto& levels = scenes.children.emplace_back("Levels", true, 8);
        levels.children.emplace_back("Level02.scene", false, 9);
        levels.children.emplace_back("Level03.scene", false, 10);

        auto& assets = root.children.emplace_back("Assets", true, 11);
        auto& textures = assets.children.emplace_back("Textures", true, 12);
        textures.children.emplace_back("player_sprite.png", false, 13);
        textures.children.emplace_back("enemy_sprite.png", false, 14);

        auto& sounds = assets.children.emplace_back("Sounds", true, 15);
        sounds.children.emplace_back("jump.wav", false, 16);
        sounds.children.emplace_back("shoot.wav", false, 17);

        root.children.emplace_back("README.md", false, 18);
        root.children.emplace_back("config.json", false, 19);

        initialized = true;
    }

    // ===== FUNCIÓN RECURSIVA =====
    std::function<void(TreeItem&)> DrawTreeNode = [&](TreeItem& item) {  // [&] captura TODO por referencia
        CustomImGui::CustomTreeNodeConfig config(item.name.c_str());

        // Iconos según tipo
        if (item.isFolder) {
            config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENES);
        } else {
            if (item.name.find(".cs") != std::string::npos) {
                config.leftIcon = FileSystemGUI::Icon(IconGUI::NODE_TYPE_DEFAULT);
            } else if (item.name.find(".scene") != std::string::npos) {
                config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENES);
            } else if (item.name.find(".png") != std::string::npos) {
                config.leftIcon = FileSystemGUI::Icon(IconGUI::IMAGE_FILE);
            } else if (item.name.find(".wav") != std::string::npos) {
                config.leftIcon = FileSystemGUI::Icon(IconGUI::NODE_EDITOR_TAB);
            } else {
                config.leftIcon = FileSystemGUI::Icon(IconGUI::NODE_TYPE_DEFAULT);
            }
        }

        config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
        config.bulletSize = ImVec2(12, 12);
        config.iconSize = ImVec2(16, 16);
        config.isLeaf = !item.isFolder;

        // ===== BOTONES CLICKABLES =====

        if (item.isFolder) {
            // CARPETAS

            // Botón: Añadir Archivo
            config.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::NODE_TYPE_DEFAULT),
                "➕ Añadir archivo",
                [&]() {  // Captura TODO por referencia
                    totalClicks++;  // Incrementar contador
                    static int counter = 100;
                    std::string newName = "nuevo_archivo_" + std::to_string(counter++) + ".txt";
                    item.children.emplace_back(newName, false, counter);

                    printf("\n╔════════════════════════════════════════╗\n");
                    printf("║  ✅ BOTÓN CLICKEADO: Añadir Archivo  ║\n");
                    printf("╚════════════════════════════════════════╝\n");
                    printf("📁 Carpeta: %s\n", item.name.c_str());
                    printf("📄 Creado: %s\n", newName.c_str());
                    printf("🔢 Clicks totales: %d\n\n", totalClicks);
                }
            );

            // Botón: Añadir Carpeta
            config.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENES),
                "➕ Añadir carpeta",
                [&]() {  // Captura TODO por referencia
                    totalClicks++;
                    static int counter = 200;
                    std::string newName = "nueva_carpeta_" + std::to_string(counter++);
                    item.children.emplace_back(newName, true, counter);

                    printf("\n╔════════════════════════════════════════╗\n");
                    printf("║  ✅ BOTÓN CLICKEADO: Añadir Carpeta  ║\n");
                    printf("╚════════════════════════════════════════╝\n");
                    printf("📁 Carpeta padre: %s\n", item.name.c_str());
                    printf("📁 Nueva: %s\n", newName.c_str());
                    printf("🔢 Clicks totales: %d\n\n", totalClicks);
                }
            );

            // Botón: Info
            config.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::PROJECT_INFO),
                "ℹ️ Información",
                [&]() {  // Captura TODO por referencia
                    totalClicks++;

                    printf("\n╔════════════════════════════════════════╗\n");
                    printf("║  ℹ️  BOTÓN CLICKEADO: Información    ║\n");
                    printf("╚════════════════════════════════════════╝\n");
                    printf("📁 Carpeta: %s\n", item.name.c_str());
                    printf("🆔 ID: %d\n", item.id);
                    printf("📦 Elementos: %zu\n", item.children.size());
                    printf("🔢 Clicks totales: %d\n\n", totalClicks);
                }
            );

            config.showChildCount = true;
            config.childCount = (int)item.children.size();

        } else {
            // ARCHIVOS

            // Botón: Abrir
            config.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::NODE_EDITOR_POPUP),
                "📂 Abrir",
                [&]() {  // Captura TODO por referencia
                    totalClicks++;

                    printf("\n╔════════════════════════════════════════╗\n");
                    printf("║  ✅ BOTÓN CLICKEADO: Abrir Archivo   ║\n");
                    printf("╚════════════════════════════════════════╝\n");
                    printf("📄 Archivo: %s\n", item.name.c_str());
                    printf("🆔 ID: %d\n", item.id);
                    printf("🔢 Clicks totales: %d\n\n", totalClicks);
                }
            );

            // Botón: Renombrar
            config.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::NODE_EDITOR_TAB),
                "✏️ Renombrar",
                [&]() {  // Captura TODO por referencia
                    totalClicks++;

                    printf("\n╔════════════════════════════════════════╗\n");
                    printf("║  ✅ BOTÓN CLICKEADO: Renombrar       ║\n");
                    printf("╚════════════════════════════════════════╝\n");
                    printf("📄 Archivo: %s\n", item.name.c_str());
                    printf("🔢 Clicks totales: %d\n\n", totalClicks);
                }
            );

            // Botón: Info
            config.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::PROJECT_INFO),
                "ℹ️ Info",
                [&]() {  // Captura TODO por referencia
                    totalClicks++;

                    printf("\n╔════════════════════════════════════════╗\n");
                    printf("║  ℹ️  BOTÓN CLICKEADO: Info Archivo   ║\n");
                    printf("╚════════════════════════════════════════╝\n");
                    printf("📄 Archivo: %s\n", item.name.c_str());
                    printf("🆔 ID: %d\n", item.id);
                    printf("🔢 Clicks totales: %d\n\n", totalClicks);
                }
            );
        }

        // Botón ELIMINAR (común para todos)
        config.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::NODE_TYPE_DEFAULT),
            "🗑️ Eliminar",
            [&]() {  // Captura TODO por referencia
                totalClicks++;

                printf("\n╔════════════════════════════════════════╗\n");
                printf("║  🗑️  BOTÓN CLICKEADO: ELIMINAR       ║\n");
                printf("╚════════════════════════════════════════╝\n");
                printf("⚠️ ADVERTENCIA!\n");
                printf("📋 Nombre: %s\n", item.name.c_str());
                printf("🆔 ID: %d\n", item.id);
                printf("📂 Tipo: %s\n", item.isFolder ? "Carpeta" : "Archivo");
                printf("🔢 Clicks totales: %d\n\n", totalClicks);
            }
        );

        // Drag & drop
        if (item.isFolder) {
            config.dragDrop = CustomImGui::TreeDragDropConfig(
                "TREE_ITEM",
                [&](void* data) {  // Captura TODO por referencia
                    TreeItem* dropped = *(TreeItem**)data;
                    printf("\n📦 DROP: '%s' → '%s'\n\n", dropped->name.c_str(), item.name.c_str());
                }
            );
        }

        config.selectedColor = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        config.hoveredColor = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);

        bool is_selected = (selectedItem == &item);
        bool isOpen = CustomImGui::CustomTreeNode(config, &is_selected);

        if (is_selected && selectedItem != &item) {
            selectedItem = &item;
            printf("🔷 SELECCIONADO: %s\n", item.name.c_str());
        }

        // Drag source
        if (ImGui::BeginDragDropSource()) {
            TreeItem* ptr = &item;
            ImGui::SetDragDropPayload("TREE_ITEM", &ptr, sizeof(TreeItem*));
            ImGui::Text("%s", item.name.c_str());
            ImGui::EndDragDropSource();
        }

        if (isOpen) {
            for (auto& child : item.children) {
                DrawTreeNode(child);
            }
            CustomImGui::CustomTreePop();
        }
    };

    // ===== UI HEADER =====
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "🎯 CONTADOR DE CLICKS: %d", totalClicks);
    ImGui::SameLine();
    if (ImGui::Button("🔄 Reset")) {
        totalClicks = 0;
        printf("\n🔄 Contador reseteado\n\n");
    }

    ImGui::Separator();

    ImGui::TextColored(ImVec4(0, 1, 0, 1), "💡 HAZ CLICK EN LOS BOTONES →");
    ImGui::BulletText("Cada click aumenta el contador");
    ImGui::BulletText("Cada click muestra un log en consola");
    ImGui::BulletText("Los botones cambian de color al hacer hover");

    ImGui::Separator();

    // Toolbar
    if (ImGui::Button("➕ Añadir a Raíz")) {
        static int counter = 300;
        root.children.emplace_back("archivo_raiz_" + std::to_string(counter), false, counter++);
        printf("➕ Añadido a raíz\n");
    }
    ImGui::SameLine();

    if (ImGui::Button("🗑️ Limpiar Todo")) {
        root.children.clear();
        selectedItem = nullptr;
        printf("🧹 Todo limpiado\n");
    }
    ImGui::SameLine();

    if (ImGui::Button("🔄 Reset Estados")) {
        CustomImGui::ResetTreeStates();
        printf("🔄 Estados reseteados\n");
    }

    ImGui::Separator();

    // ===== ÁRBOL =====
    ImGui::BeginChild("TreeView", ImVec2(0, -40), true);
    DrawTreeNode(root);
    ImGui::EndChild();

    // ===== INFO =====
    ImGui::Separator();
    if (selectedItem) {
        ImGui::Text("Seleccionado: %s (ID: %d)", selectedItem->name.c_str(), selectedItem->id);
    } else {
        ImGui::TextDisabled("Nada seleccionado");
    }

    ImGui::End();
}


