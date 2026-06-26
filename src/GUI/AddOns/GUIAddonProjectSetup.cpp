//
// Created by Eduardo on 06/12/2025.
//

#include <algorithm>
#include "../include/Brakeza.h"
#include "../../../include/Components/Components.h"
#include "../../../include/GUI/AddOns/GUIAddonProjectSetup.h"
#include "../../../include/GUI/AddOns/CustomTreeNode.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/Render/Drawable.h"

// Static member definitions
bool GUIAddonProjectSetup::openPopupEditProject = false;
bool GUIAddonProjectSetup::openPopupAdditiveScene = false;
bool GUIAddonProjectSetup::additiveLoadScripts = false;
bool GUIAddonProjectSetup::additiveLoadShaders = false;
bool GUIAddonProjectSetup::additiveLoadCamera = false;
bool GUIAddonProjectSetup::additiveLoadRenderSettings = false;
static std::string s_additiveScenePath;

void GUIAddonProjectSetup::DrawWinProjectSettings()
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::PROJECT_SETTINGS);
    if (!windowStatus->isOpen) return;

    auto scripting = Components::get()->Scripting();
    auto project = scripting->getCurrentProject();
    auto scene = scripting->getCurrentScene();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 4.0f));

    // ============================================
    // NODO 1: PROJECT
    // ============================================
    {
        std::string projectLabel = project != nullptr
            ? "Project: " + project->getChecker().getStatus().name
            : "Project: Unknown";

        CustomImGui::CustomTreeNodeConfig projectConfig(projectLabel.c_str());

        projectConfig.leftIcon = FileSystemGUI::Icon(project != nullptr ? IconGUI::PROJECT_FILE : IconGUI::PROJECT_FILE_UNNAMED);
        projectConfig.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        projectConfig.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
        projectConfig.defaultOpen = false;
        projectConfig.forceOpenPtr = &FileSystemGUI::autoExpandProject;

        // Color según estado
        if (project != nullptr) {
            projectConfig.selectedColor = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
            projectConfig.hoveredColor = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
        } else {
            projectConfig.selectedColor = ImVec4(0.3f, 0.3f, 0.3f, 0.2f);
            projectConfig.hoveredColor = ImVec4(0.3f, 0.3f, 0.3f, 0.1f);
        }

        // Botones de acción
        if (project != nullptr) {
            projectConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::PROJECT_INFO),
                "Project info",
                [&]() {
                    Brakeza::get()->GUI()->getProjectChecker().LoadProjectInfoDialog(project->getFilePath());
                }
            );
            projectConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::SCRIPT_EDIT),
                "Edit project",
                [project]() {
                    project->resetEditBuffers();
                    openPopupEditProject = true;
                }
            );
            projectConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::PROJECT_SAVE),
                "Save project",
                [&]() {
                    ProjectLoader::SaveProject(project->getFilePath());
                },
                "Save Project",
                "This will overwrite the saved project file. Are you sure?"
            );
            projectConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::PROJECT_CLOSE),
                "Close project",
                [&]() {
                    ProjectLoader::CloseCurrentProject();
                },
                "Close Project",
                "Any unsaved changes will be lost. Are you sure?"
            );
        } else {
            projectConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::CREATE_FILE),
                "New project",
                []() {
                    FileSystemGUI::type = GUIType::BROWSE_PROJECTS;
                    FileSystemGUI::openPopUpCreateProject = true;
                }
            );
        }

        bool isProjectOpen = CustomImGui::CustomTreeNode(projectConfig, nullptr);

        if (isProjectOpen) {
            ImGui::Spacing();
            if (project != nullptr) {
                DrawProjectSettingsNode();
                ImGui::Spacing();
                DrawProjectScriptsNode();
                ImGui::Spacing();
                DrawProjectScenesNode();
            } else {
                Drawable::WarningMessage("No project loaded");
            }
            CustomImGui::CustomTreePop();
        }
        ImGui::Spacing();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // ============================================
    // NODO 2: ESCENAS CARGADAS
    // ============================================
    auto &loadedScenes = scripting->getLoadedScenes();

    if (loadedScenes.empty()) {
        CustomImGui::CustomTreeNodeConfig sceneConfig("Scene: Unknown");
        sceneConfig.leftIcon = FileSystemGUI::Icon(IconGUI::SCENE_FILE_UNNAMED);
        sceneConfig.isLeaf = true;
        CustomImGui::CustomTreeNode(sceneConfig, nullptr);
    }

    for (int si = 0; si < (int)loadedScenes.size(); si++) {
        auto *loadedScene = loadedScenes[si];
        ImGui::PushID(si);

        std::string sceneLabel = "Scene: " + Tools::removeSubstring(loadedScene->getFilePath(), Config::get()->ASSETS_FOLDER);
        if (!loadedScene->isActive()) sceneLabel += "  [inactive]";

        CustomImGui::CustomTreeNodeConfig sceneConfig(sceneLabel.c_str());
        sceneConfig.leftIcon = FileSystemGUI::Icon(IconGUI::SCENE_FILE);
        sceneConfig.isLeaf = true;
        sceneConfig.defaultOpen = false;

        sceneConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCENE_INFO),
            "Scene detail",
            [loadedScene]() {
                Brakeza::get()->GUI()->getSceneChecker().LoadSceneInfoDialog(loadedScene->getFilePath());
            }
        );
        sceneConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCENE_SAVE),
            "Save scene",
            [loadedScene]() {
                SceneLoader::SaveScene(loadedScene->getFilePath());
            },
            "Save Scene",
            "This will overwrite the saved scene file. Are you sure?"
        );
        sceneConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCENE_LOAD),
            "Reload scene",
            [loadedScene]() {
                SceneLoader::ReloadScene(loadedScene->getName());
            }
        );

        sceneConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCENE_UNLOAD),
            "Unload scene",
            [loadedScene]() {
                SceneLoader::UnloadScene(loadedScene->getName());
            },
            "Unload Scene",
            "This will remove all objects belonging to this scene. Are you sure?"
        );

        bool isTemporary = !scripting->hasProjectScene(loadedScene->getFilePath());
        if (isTemporary && project != nullptr) {
            sceneConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::SCENE_ATTACH_PROJECT),
                "Attach scene to Project",
                [loadedScene]() {
                    Components::get()->Scripting()->AddProjectScene(loadedScene->getFilePath());
                }
            );
        }

        CustomImGui::CustomTreeNode(sceneConfig, nullptr);

        ImGui::PopID();
    }

    ImGui::Separator();
    ImGui::Spacing();

    DrawSceneRenderSettingsNode();
    ImGui::Spacing();

    DrawSceneScriptsNode();
    ImGui::Spacing();

    DrawSceneShadersNode();
    ImGui::Spacing();

    ImGui::PopStyleVar();

    // Draw edit popup if open
    DrawProjectEditPopup();
    DrawAdditiveScenePopup();
}

// ============================================
// SUB-NODOS DE PROJECT
// ============================================

void GUIAddonProjectSetup::DrawProjectSettingsNode()
{
    auto scripting = Components::get()->Scripting();
    auto project = scripting->getCurrentProject();

    if (project == nullptr) return;

    CustomImGui::CustomTreeNodeConfig config("Project settings");
    config.iconSize = ImVec2(18, 18);
    config.leftIcon = FileSystemGUI::Icon(IconGUI::WIN_PROJECT_SETTINGS);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    config.defaultOpen = false;

    bool isOpen = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpen) {
        ImGui::Spacing();
        DrawProjectSettings();
        CustomImGui::CustomTreePop();
    }
}

void GUIAddonProjectSetup::DrawProjectScriptsNode()
{
    static bool shouldOpen = false;

    auto scripting = Components::get()->Scripting();
    auto scripts = scripting->getProjectScripts();

    std::string label = "Project scripts";

    CustomImGui::CustomTreeNodeConfig config(label.c_str());
    config.iconSize = ImVec2(18, 18);
    config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_GLOBAL_SCRIPTS);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    config.defaultOpen = shouldOpen;
    config.showChildCount = true;
    config.childCount = (int) scripts.size();

    config.dragDrop = CustomImGui::TreeDragDropConfig(
        GUIType::DragDropTarget::SCRIPT_ITEM,
        [&](void* data) {
            LOG_MESSAGE("Dropping script (%s) in project space", (char*)data);
            auto meta = ScriptLuaGUI::ExtractScriptMetainfo(std::string((char*)data));
            auto script = new ScriptLUA(meta.name, meta.codeFile, meta.typesFile);
            if (script->getType() != SCRIPT_GLOBAL) {
                LOG_ERROR("[Project] Error: Cannot attach Object script to Project. Only Global scripts are allowed.");
                delete script;
                return;
            }
            scripting->AddProjectLUAScript(script);
            shouldOpen = true;
        }
    );

    bool isOpen = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpen) {
        if (shouldOpen) shouldOpen = false;

        if (scripts.empty()) {
            ImGui::Spacing();
            Drawable::WarningMessage("There are not scripts attached to project");
        } else {
            DrawProjectScripts();
        }
        CustomImGui::CustomTreePop();
    }
}

void GUIAddonProjectSetup::DrawProjectScenesNode()
{
    static bool shouldOpen = false;

    auto scripting = Components::get()->Scripting();
    auto scenes = scripting->getProjectScenes();

    std::string label = "Project scenes";

    CustomImGui::CustomTreeNodeConfig config(label.c_str());
    config.iconSize = ImVec2(18, 18);
    config.leftIcon = FileSystemGUI::Icon(IconGUI::SCENE_FILE);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    config.defaultOpen = shouldOpen;
    config.showChildCount = true;
    config.childCount = (int)scenes.size();

    config.dragDrop = CustomImGui::TreeDragDropConfig(
        GUIType::DragDropTarget::SCENE_ITEM,
        [&](void* data) {
            const char* scenePath = (const char*)data;
            LOG_MESSAGE("Dropping scene '%s' in project space", scenePath);
            scripting->AddProjectScene(scenePath);
            shouldOpen = true;
        }
    );

    bool isOpen = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpen) {
        if (shouldOpen) shouldOpen = false;

        ImGui::Spacing();
        if (scenes.empty()) {
            Drawable::WarningMessage("There are not scenes attached");
        } else {
            DrawProjectScenes();
        }
        CustomImGui::CustomTreePop();
    }
}

// ============================================
// SUB-NODOS DE SCENE ACTUAL
// ============================================

void GUIAddonProjectSetup::DrawSceneScriptsNode()
{
    static bool shouldOpen = false;

    auto scripting = Components::get()->Scripting();
    auto scripts = scripting->getSceneScripts();

    std::string label = "Scene scripts";

    CustomImGui::CustomTreeNodeConfig config(label.c_str());
    config.iconSize = ImVec2(18, 18);
    config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENE_SCRIPTS);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    config.defaultOpen = shouldOpen;
    config.showChildCount = true;
    config.childCount = (int)scripts.size();

    config.dragDrop = CustomImGui::TreeDragDropConfig(
        GUIType::DragDropTarget::SCRIPT_ITEM,
        [&](void* data) {
            LOG_MESSAGE("Dropping script (%s) in scene space", (char*)data);
            auto meta = ScriptLuaGUI::ExtractScriptMetainfo(std::string((char*)data));
            auto script = new ScriptLUA(meta.name, meta.codeFile, meta.typesFile);
            if (script->getType() != SCRIPT_GLOBAL) {
                LOG_ERROR("[Scene] Error: Cannot attach Object script to Scene. Only Global scripts are allowed.");
                delete script;
                return;
            }
            scripting->AddSceneLUAScript(script);
            shouldOpen = true;
        }
    );

    bool isOpen = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpen) {
        if (shouldOpen) shouldOpen = false;
        ImGui::Spacing();

        if (scripts.empty()) {
            Drawable::WarningMessage("There are not scenes attached");
        } else {
            DrawSceneScripts();
        }
        CustomImGui::CustomTreePop();
    }
}

void GUIAddonProjectSetup::DrawSceneShadersNode()
{
    static bool shouldOpen = false;

    auto render = Components::get()->Render();
    auto shaders = render->getSceneShaders();

    std::string label = "Scene shaders";

    CustomImGui::CustomTreeNodeConfig config(label.c_str());
    config.iconSize = ImVec2(18, 18);
    config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENE_SHADERS);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    config.defaultOpen = shouldOpen;
    config.showChildCount = true;
    config.childCount = (int)shaders.size();

    config.dragDrop = CustomImGui::TreeDragDropConfig(
        GUIType::DragDropTarget::SHADER_ITEM,
        [&](void* data) {
            auto receivedData = (Config::DragDropCustomShaderData*)data;
            auto fullPath = std::string(receivedData->folder) + receivedData->file;
            LOG_MESSAGE("Dropping shader file '%s' in scene space...", fullPath.c_str());
            render->LoadShaderIntoScene(fullPath);
            shouldOpen = true;
        }
    );

    bool postProcessEnabled = Config::get()->ENABLE_POST_PROCESSING_CHAIN;
    config.actionItems.emplace_back(
        FileSystemGUI::Icon(postProcessEnabled ? IconGUI::SHADER_UNLOCK : IconGUI::SHADER_LOCK),
        postProcessEnabled ? "Disable Post-Processing Chain" : "Enable Post-Processing Chain",
        []() {
            Config::get()->ENABLE_POST_PROCESSING_CHAIN = !Config::get()->ENABLE_POST_PROCESSING_CHAIN;
        }
    );

    config.actionItems.emplace_back(
        FileSystemGUI::Icon(IconGUI::SHADER_RELOAD),
        "Reload all scene shaders",
        []() {
            auto render = Components::get()->Render();
            for (auto& s : render->getSceneShaders()) {
                s->Reload();
            }
            auto* pm = Components::get()->Window()->getPostProcessingManager();
            pm->rebuildFBOs();
            LOG_MESSAGE("[ProjectSetup] All scene shaders reloaded, FBOs rebuilt");
        }
    );

    bool isOpen = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpen) {
        if (shouldOpen) shouldOpen = false;
        ImGui::Spacing();

        if (shaders.empty()) {
            Drawable::WarningMessage("There are not shaders attached");
        } else {
            DrawSceneCustomShaders();
        }
        CustomImGui::CustomTreePop();
    }
}

// ============================================
// DRAW FUNCTIONS
// ============================================

void GUIAddonProjectSetup::DrawProjectScenes()
{
    auto scripting = Components::get()->Scripting();
    auto scenes = scripting->getProjectScenes();
    std::sort(scenes.begin(), scenes.end());

    for (unsigned int i = 0; i < scenes.size(); i++) {
        auto currentScene = scenes[i];
        ImGui::PushID(i);
        auto label = Tools::removeSubstring(currentScene, Config::get()->ASSETS_FOLDER);

        CustomImGui::CustomTreeNodeConfig sceneConfig(label.c_str());
        sceneConfig.leftIcon = FileSystemGUI::Icon(IconGUI::SCENE_FILE);
        sceneConfig.iconSize = ImVec2(18, 18);
        sceneConfig.textColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

        // BUTTONS
        sceneConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCENE_LOAD),
            "Load Scene",
            [currentScene]() {
                SceneLoader::ClearWorld();
                SceneLoader::LoadScene(currentScene);
            }
        );

        sceneConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCENE_ATTACH_PROJECT),
            "Load additive",
            [currentScene]() {
                s_additiveScenePath = currentScene;
                GUIAddonProjectSetup::additiveLoadScripts = false;
                GUIAddonProjectSetup::additiveLoadShaders = false;
                GUIAddonProjectSetup::additiveLoadCamera = false;
                GUIAddonProjectSetup::additiveLoadRenderSettings = false;
                GUIAddonProjectSetup::openPopupAdditiveScene = true;
            }
        );

        sceneConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::LUA_REMOVE),
            "Remove scene from project",
            [scripting, currentScene]() {
                scripting->RemoveProjectScene(currentScene);
            },
            "Remove Scene",
            "This will detach the scene from the project. Are you sure?"
        );

        bool isOpen = CustomImGui::CustomTreeNode(sceneConfig, nullptr);
        if (isOpen) {
            // Las escenas no tienen propiedades que mostrar, así que cerramos inmediatamente
            CustomImGui::CustomTreePop();
        }

        ImGui::PopID();
    }
}

void GUIAddonProjectSetup::DrawProjectSettings()
{
    auto scripting = Components::get()->Scripting();
    auto project = scripting->getCurrentProject();

    if (project == nullptr) {
        Drawable::WarningMessage("There are not a project loaded");
        return;
    }

    auto checker = scripting->getCurrentProject()->getChecker();
    if (!checker.isLoaded()) return;

    std::string projectName = checker.getStatus().name;
    std::string filePath = Tools::removeSubstring(checker.getFilePath(), Config::get()->ASSETS_FOLDER);

    Vertex3D gravity = checker.getStatus().gravity;

    int resolutionW = checker.getStatus().resolution.width;
    int resolutionH = checker.getStatus().resolution.height;

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
    auto scripts = scripting->getProjectScripts();
    ImGui::Spacing();
    for (unsigned int i = 0; i < scripts.size(); i++) {
        auto currentScript = scripts[i];
        ImGui::PushID(i);

        // Evaluar estados ANTES de crear el config
        bool isPaused = currentScript->isPaused();
        std::string scriptName = currentScript->getName();

        // Configurar CustomTreeNode para cada script
        CustomImGui::CustomTreeNodeConfig scriptConfig(scriptName.c_str());
        scriptConfig.iconSize = ImVec2(18, 18);
        scriptConfig.leftIcon = FileSystemGUI::Icon(IconGUI::SCRIPT_FILE);
        scriptConfig.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        scriptConfig.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
        scriptConfig.defaultOpen = false;

        // Botones de acción
        scriptConfig.actionItems.emplace_back(
            isPaused ? FileSystemGUI::Icon(IconGUI::LUA_LOCK) : FileSystemGUI::Icon(IconGUI::LUA_UNLOCK),
            isPaused ? "Unlock project script" : "Lock project script",
            [currentScript]() { currentScript->setPaused(!currentScript->isPaused()); }
        );

        scriptConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCRIPT_RELOAD),
            "Reload project script",
            [currentScript]() { currentScript->Reload(); }
        );

        scriptConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::LUA_REMOVE),
            "Remove script",
            [scripting, currentScript]() { scripting->RemoveProjectScript(currentScript); },
            "Remove Script",
            "This will detach the script from the project. Are you sure?"
        );

        scriptConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::ARROW_UP),
            "Move up",
            [scripting, currentScript]() { scripting->MoveProjectScriptUp(currentScript); }
        );

        scriptConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::ARROW_DOWN),
            "Move down",
            [scripting, currentScript]() { scripting->MoveProjectScriptDown(currentScript); }
        );

        bool isOpenCurrentScript = CustomImGui::CustomTreeNode(scriptConfig, nullptr);

        if (isOpenCurrentScript) {
            currentScript->DrawImGuiProperties();
            CustomImGui::CustomTreePop();
        }

        ImGui::PopID();
    }
}

void GUIAddonProjectSetup::DrawSceneScripts()
{
    auto scripting = Components::get()->Scripting();
    auto scripts = scripting->getSceneScripts();

    for (unsigned int i = 0; i < scripts.size(); i++) {
        auto currentScript = scripts[i];
        ImGui::PushID(i);

        // Evaluar estados ANTES de crear el config
        bool isPaused = currentScript->isPaused();
        auto *scriptScene = currentScript->getScene();
        std::string sceneSuffix = scriptScene ? std::string("  [") + scriptScene->getName() + "]" : "";
        std::string scriptName = currentScript->getName() + sceneSuffix;

        // Configurar CustomTreeNode para cada script
        CustomImGui::CustomTreeNodeConfig scriptConfig(scriptName.c_str());
        scriptConfig.leftIcon = FileSystemGUI::Icon(IconGUI::SCRIPT_FILE);
        scriptConfig.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        scriptConfig.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
        scriptConfig.iconSize = ImVec2(18, 18);
        scriptConfig.defaultOpen = false;

        // Botones de acción - captura SOLO el puntero, no evalúes estados dinámicos aquí
        scriptConfig.actionItems.emplace_back(
            isPaused ? FileSystemGUI::Icon(IconGUI::LUA_LOCK) : FileSystemGUI::Icon(IconGUI::LUA_UNLOCK),
            isPaused ? "Unlock scene script" : "Lock scene script",
            [currentScript]() { currentScript->setPaused(!currentScript->isPaused()); }
        );

        scriptConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCRIPT_EDIT),
            "Edit script",
            [currentScript]() { ScriptLuaGUI::LoadScriptDialog(currentScript->getTypesFile()); }
        );

        scriptConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCRIPT_RELOAD),
            "Reload scene script",
            [currentScript]() { currentScript->Reload(); }
        );

        scriptConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::LUA_REMOVE),
            "Remove scene script",
            [scripting, currentScript]() { scripting->RemoveSceneScript(currentScript); },
            "Remove Script",
            "This will detach the script from the scene. Are you sure?"
        );

        scriptConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::ARROW_UP),
            "Move up",
            [scripting, currentScript]() { scripting->MoveSceneScriptUp(currentScript); }
        );

        scriptConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::ARROW_DOWN),
            "Move down",
            [scripting, currentScript]() { scripting->MoveSceneScriptDown(currentScript); }
        );

        bool isOpenCurrentScript = CustomImGui::CustomTreeNode(scriptConfig, nullptr);

        if (isOpenCurrentScript) {
            currentScript->DrawImGuiProperties();
            CustomImGui::CustomTreePop();
        }

        ImGui::PopID();
    }
}

void GUIAddonProjectSetup::DrawSceneRenderSettingsNode()
{
    CustomImGui::CustomTreeNodeConfig config("Render settings");
    config.iconSize    = ImVec2(18, 18);
    config.leftIcon    = FileSystemGUI::Icon(IconGUI::WIN_PROJECT_SETTINGS);
    config.bulletOpen  = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    config.defaultOpen = false;

    bool isOpen = CustomImGui::CustomTreeNode(config, nullptr);
    if (!isOpen) return;

    ImGui::Spacing();

    auto win = Components::get()->Window();
    float col[4] = { win->getClearColorR(), win->getClearColorG(), win->getClearColorB(), win->getClearColorA() };

    ImGui::Image(FileSystemGUI::Icon(IconGUI::BULLET), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Clear color");
    ImGui::SetNextItemWidth(-1);
    if (ImGui::ColorEdit4("##ClearColor", col, ImGuiColorEditFlags_AlphaBar)) {
        win->setClearColor(col[0], col[1], col[2], col[3]);
    }

    ImGui::Spacing();
    CustomImGui::CustomTreePop();
}

void GUIAddonProjectSetup::DrawSceneCustomShaders()
{
    auto render = Components::get()->Render();
    auto shaders = render->getSceneShaders();

    for (int i = 0; i < (int)shaders.size(); i++) {
        auto currentShader = shaders[i];
        ImGui::PushID(i);

        // Evaluar estados ANTES de crear el config
        bool isEnabled = currentShader->isEnabled();
        auto *shaderScene = currentShader->getScene();
        std::string sceneSuffix = shaderScene ? std::string("  [") + shaderScene->getName() + "]" : "";
        std::string shaderLabel = currentShader->getLabel() + sceneSuffix;

        // Configurar CustomTreeNode para cada shader
        CustomImGui::CustomTreeNodeConfig shaderConfig(shaderLabel.c_str());
        shaderConfig.leftIcon = FileSystemGUI::Icon(IconGUI::SHADER_FILE);
        shaderConfig.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        shaderConfig.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
        shaderConfig.iconSize = ImVec2(18, 18);
        shaderConfig.defaultOpen = false;

        bool isNodeShader = currentShader->getType() == SHADER_NODE_OBJECT || currentShader->getType() == SHADER_NODE_POSTPROCESSING;

        // Botones de acción
        shaderConfig.actionItems.emplace_back(
            !isEnabled ? FileSystemGUI::Icon(IconGUI::SHADER_LOCK) : FileSystemGUI::Icon(IconGUI::SHADER_UNLOCK),
            !isEnabled ? "Unlock scene shader" : "Lock scene shader",
            [currentShader]() { currentShader->setEnabled(!currentShader->isEnabled()); }
        );

        shaderConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(isNodeShader ? IconGUI::WIN_SHADER_NODES: IconGUI::SHADER_EDIT),
            "Edit shader",
            [currentShader]() { ShadersGUI::LoadDialogShader(currentShader); }
        );

        shaderConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::LUA_RELOAD),
            "Reload shader",
            [currentShader]() { currentShader->Reload(); }
        );

        int index = i;
        shaderConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::LUA_REMOVE),
            "Remove shader from scene",
            [render, index]() { render->RemoveSceneShaderByIndex(index); },
            "Remove Shader",
            "This will detach the shader from the scene. Are you sure?"
        );

        shaderConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::ARROW_UP),
            "Move up",
            [render, currentShader]() { render->MoveSceneShaderUp(currentShader); }
        );

        shaderConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::ARROW_DOWN),
            "Move down",
            [render, currentShader]() { render->MoveSceneShaderDown(currentShader); }
        );

        bool isOpenCurrentShader = CustomImGui::CustomTreeNode(shaderConfig, nullptr);

        if (isOpenCurrentShader) {
            currentShader->DrawImGuiProperties(nullptr, nullptr);
            CustomImGui::CustomTreePop();
        }

        ImGui::PopID();
    }
}

void GUIAddonProjectSetup::DrawProjectEditPopup()
{
    if (openPopupEditProject) {
        ImGui::OpenPopup("Edit Project");
        openPopupEditProject = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(450, 300), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal("Edit Project", nullptr, ImGuiWindowFlags_None)) {
        auto scripting = Components::get()->Scripting();
        auto project = scripting->getCurrentProject();

        if (project == nullptr) {
            ImGui::Text("No project loaded");
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
            return;
        }

        ImGui::Spacing();

        // Project Name input
        ImGui::Text("Project Name:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##ProjectName", project->editName, sizeof(project->editName));

        ImGui::Spacing();
        ImGui::Spacing();

        // Project Description input
        ImGui::Text("Description:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextMultiline("##ProjectDescription", project->editDescription, sizeof(project->editDescription),
            ImVec2(-1, 100), ImGuiInputTextFlags_AllowTabInput);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Buttons
        float buttonWidth = 100.0f;
        float spacing = 10.0f;
        float totalWidth = buttonWidth * 2 + spacing;
        float startX = (ImGui::GetWindowWidth() - totalWidth) * 0.5f;

        ImGui::SetCursorPosX(startX);

        if (ImGui::Button("Save", ImVec2(buttonWidth, 0))) {
            project->applyEditBuffers();

            // Update window title
            Config::get()->ENGINE_TITLE = project->getName();
            Components::get()->Window()->setWindowTitle(project->getName().c_str());

            // Save project to disk
            ProjectLoader::SaveProject(project->getFilePath());

            // Reload checker to reflect changes
            project->reloadChecker();

            LOG_MESSAGE("[Project] Project updated: %s", project->getName().c_str());
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine(0, spacing);

        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GUIAddonProjectSetup::DrawAdditiveScenePopup()
{
    if (openPopupAdditiveScene) {
        ImGui::OpenPopup("Load Additive Scene");
        openPopupAdditiveScene = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(280, 0), ImGuiCond_Always);

    if (ImGui::BeginPopupModal("Load Additive Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::TextUnformatted("Select what to load from the scene:");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Scripts",         &additiveLoadScripts);
        ImGui::Checkbox("Shaders",         &additiveLoadShaders);
        ImGui::Checkbox("Camera",          &additiveLoadCamera);
        ImGui::Checkbox("Render settings", &additiveLoadRenderSettings);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Load", ImVec2(120, 0))) {
            SceneLoader::LoadSceneAdditive(
                FilePath::SceneFile(s_additiveScenePath),
                additiveLoadScripts,
                additiveLoadShaders,
                additiveLoadCamera,
                additiveLoadRenderSettings
            );
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::Spacing();
        ImGui::EndPopup();
    }
}