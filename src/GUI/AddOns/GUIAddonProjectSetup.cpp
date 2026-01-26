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
                FileSystemGUI::Icon(IconGUI::PROJECT_SAVE),
                "Save project",
                [&]() {
                    ProjectLoader::SaveProject(project->getFilePath());
                }
            );
            projectConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::PROJECT_CLOSE),
                "Close project",
                [&]() {
                    ProjectLoader::CloseCurrentProject();
                }
            );
        } else {
            projectConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::CREATE_FILE),
                "New project",
                [&]() {
                    printf("TODO: Nuevo proyecto\n");
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
    // NODO 2: SCENE ACTUAL
    // ============================================
    {
        std::string sceneLabel = scene != nullptr
            ? "Scene: " + scene->getFilePath()
            : "Scene: Unknown";

        CustomImGui::CustomTreeNodeConfig sceneConfig(sceneLabel.c_str());

        // Icono según estado
        sceneConfig.leftIcon = FileSystemGUI::Icon(scene != nullptr ? IconGUI::SCENE_FILE : IconGUI::SCENE_FILE_UNNAMED);
        sceneConfig.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        sceneConfig.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
        sceneConfig.defaultOpen = false;

        bool isTemporary = false;

        // Botones de acción
        if (scene != nullptr) {
            // Capturar por valor las variables necesarias
            auto currentScene = scene;

            sceneConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::SCENE_INFO),
                "Scene detail",
                [currentScene]() {
                    LOG_MESSAGE("wtf");
                    Brakeza::get()->GUI()->getSceneChecker().LoadSceneInfoDialog(currentScene->getFilePath());
                }
            );
            sceneConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::SCENE_SAVE),
                "Save scene",
                [currentScene]() {
                    SceneLoader::SaveScene(currentScene->getFilePath());
                }
            );

            isTemporary = !Components::get()->Scripting()->hasProjectScene(scene->getFilePath());
        }

        if (isTemporary && project != nullptr) {
            // Capturar por valor las variables necesarias
            auto currentScene = scene;

            sceneConfig.actionItems.emplace_back(
                FileSystemGUI::Icon(IconGUI::SCENE_ATTACH_PROJECT),
                "Attach scene to Project",
                [currentScene]() {
                    LOG_ERROR("HOLA");
                    Components::get()->Scripting()->AddProjectScene(currentScene->getFilePath());
                }
            );
        }

        bool isSceneOpen = CustomImGui::CustomTreeNode(sceneConfig, nullptr);

        if (isSceneOpen) {
            ImGui::Spacing();
            if (isTemporary) {
                Drawable::WarningMessage("Scene not belonging to the project");
            }

            DrawSceneScriptsNode();
            ImGui::Spacing();

            DrawSceneShadersNode();
            ImGui::Spacing();

            CustomImGui::CustomTreePop();
        }
    }

    ImGui::PopStyleVar();
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
    config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_GLOBAL_SCRIPTS);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    config.defaultOpen = shouldOpen;
    config.showChildCount = true;
    config.childCount = (int)scripts.size();

    config.dragDrop = CustomImGui::TreeDragDropConfig(
        GUIType::DragDropTarget::SCRIPT_ITEM,
        [&](void* data) {
            LOG_MESSAGE("Dropping script (%s) in project space", (char*)data);
            auto meta = ScriptLuaGUI::ExtractScriptMetainfo(std::string((char*)data));
            scripting->AddProjectLUAScript(new ScriptLUA(meta.name, meta.codeFile, meta.typesFile));
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
            scripting->AddSceneLUAScript(new ScriptLUA(meta.name, meta.codeFile, meta.typesFile));
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
// FUNCIONES TREE (REFACTORIZADAS CON CustomTreeNode)
// ============================================

void GUIAddonProjectSetup::TreeSceneScripts()
{
    auto scripting = Components::get()->Scripting();
    static bool shouldOpen = false;

    std::string labelSceneScripts = "Scene scripts (" + std::to_string(scripting->getSceneScripts().size()) + ")";

    CustomImGui::CustomTreeNodeConfig config(labelSceneScripts.c_str());
    config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENE_SCRIPTS);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    //config.bulletSize = ImVec2(18, 18);
    config.defaultOpen = shouldOpen;
    config.showChildCount = true;
    config.childCount = (int)scripting->getSceneScripts().size();

    config.dragDrop = CustomImGui::TreeDragDropConfig(
        GUIType::DragDropTarget::SCRIPT_ITEM,
        [&](void* data) {
            LOG_MESSAGE("Dropping script (%s) in scene space", (char*)data);
            auto meta = ScriptLuaGUI::ExtractScriptMetainfo(std::string((char*)data));
            scripting->AddSceneLUAScript(new ScriptLUA(meta.name, meta.codeFile, meta.typesFile));
            shouldOpen = true;
        }
    );

    bool isOpenSceneScripts = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpenSceneScripts) {
        if (shouldOpen) shouldOpen = false;

        DrawSceneScripts();
        if (scripting->getSceneScripts().empty()) {
            Drawable::WarningMessage("There are not scripts attached");
        }
        CustomImGui::CustomTreePop();
    }
}

void GUIAddonProjectSetup::TreeProjectScenes()
{
    auto scripting = Components::get()->Scripting();
    static bool shouldOpen = false;

    auto projectScenes = scripting->getProjectScenes();
    std::string labelProjectScenes = "Project scenes (" + std::to_string(projectScenes.size()) + ")";

    CustomImGui::CustomTreeNodeConfig config(labelProjectScenes.c_str());
    config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENES);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    //config.bulletSize = ImVec2(18, 18);
    config.defaultOpen = shouldOpen;
    config.showChildCount = true;
    config.childCount = (int)projectScenes.size();

    config.dragDrop = CustomImGui::TreeDragDropConfig(
        GUIType::DragDropTarget::SCENE_ITEM,
        [&](void* data) {
            const char* scenePath = (const char*)data;
            LOG_MESSAGE("Dropping scene '%s' in project space", scenePath);
            scripting->AddProjectScene(scenePath);
            shouldOpen = true;
        }
    );

    bool isOpenProjectScenes = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpenProjectScenes) {
        if (shouldOpen) shouldOpen = false;

        DrawProjectScenes();
        if (projectScenes.empty()) {
            Drawable::WarningMessage("There are not scenes attached");
        }
        CustomImGui::CustomTreePop();
    }
}

void GUIAddonProjectSetup::TreeProjectSettings()
{
    std::string labelProjectSettings = "Project settings";

    CustomImGui::CustomTreeNodeConfig config(labelProjectSettings.c_str());
    config.leftIcon = FileSystemGUI::Icon(IconGUI::WIN_PROJECT_SETTINGS);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    //config.bulletSize = ImVec2(18, 18);
    config.defaultOpen = false;

    bool isOpen = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpen) {
        DrawProjectSettings();
        CustomImGui::CustomTreePop();
    }
}

void GUIAddonProjectSetup::TreeProjectScripts()
{
    auto scripting = Components::get()->Scripting();
    static bool shouldOpen = false;

    std::string labelGlobalScripts = "Project scripts";

    CustomImGui::CustomTreeNodeConfig config(labelGlobalScripts.c_str());
    config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_GLOBAL_SCRIPTS);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    config.defaultOpen = shouldOpen;
    config.showChildCount = true;
    config.childCount = (int)scripting->getProjectScripts().size();

    config.dragDrop = CustomImGui::TreeDragDropConfig(
        GUIType::DragDropTarget::SCRIPT_ITEM,
        [&](void* data) {
            LOG_MESSAGE("Dropping script '%s' in project space", (char*)data);
            auto meta = ScriptLuaGUI::ExtractScriptMetainfo(std::string((char*)data));
            scripting->AddProjectLUAScript(new ScriptLUA(meta.name, meta.codeFile, meta.typesFile));
            shouldOpen = true;
        }
    );

    bool isOpenGlobalScripts = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpenGlobalScripts) {

        if (shouldOpen) shouldOpen = false;
        ImGui::Spacing();

        DrawProjectScripts();
        if (scripting->getProjectScripts().empty()) {
            Drawable::WarningMessage("There are not scripts attached");
        }
        CustomImGui::CustomTreePop();
    }
}

void GUIAddonProjectSetup::TreeSceneShaders()
{
    auto render = Components::get()->Render();
    static bool shouldOpen = false;

    std::string labelSceneShaders = "Scene shaders (" + std::to_string(render->getSceneShaders().size()) + ")";

    CustomImGui::CustomTreeNodeConfig config(labelSceneShaders.c_str());
    config.leftIcon = FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENE_SHADERS);
    config.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    config.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    //config.bulletSize = ImVec2(18, 18);
    config.defaultOpen = shouldOpen;
    config.showChildCount = true;
    config.childCount = (int)render->getSceneShaders().size();

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

    bool isOpenSceneShaders = CustomImGui::CustomTreeNode(config, nullptr);

    if (isOpenSceneShaders) {
        if (shouldOpen) shouldOpen = false;

        DrawSceneCustomShaders();
        if (render->getSceneShaders().empty()) {
            Drawable::WarningMessage("There are not shaders attached");
        }
        CustomImGui::CustomTreePop();
    }
}

// ============================================
// BOTONES DE GUARDADO
// ============================================

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

// ============================================
// DRAW FUNCTIONS
// ============================================

void GUIAddonProjectSetup::DrawProjectScenes()
{
    auto scripting = Components::get()->Scripting();
    auto scenes = scripting->getProjectScenes();

    for (unsigned int i = 0; i < scenes.size(); i++) {
        auto currentScene = scenes[i];
        ImGui::PushID(i);

        // Configurar CustomTreeNode para cada escena
        CustomImGui::CustomTreeNodeConfig sceneConfig(currentScene.c_str());
        sceneConfig.leftIcon = FileSystemGUI::Icon(IconGUI::SCENE_FILE);

        // Botones de acción
        sceneConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCENE_LOAD),
            "Load Scene",
            [currentScene]() {
                SceneLoader::ClearScene();
                SceneLoader::LoadScene(currentScene);
            }
        );

        sceneConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::LUA_REMOVE),
            "Remove scene from project",
            [scripting, currentScene]() {
                scripting->RemoveProjectScene(currentScene);
            }
        );

        // Como las escenas son solo strings, no necesitan ser expandibles
        // Puedes usar un TreeNode simple o solo mostrar el item con botones
        // Si quieres mantener la apariencia de TreeNode (aunque no se expanda):
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
    std::string filePath = checker.getFilePath();
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
            [scripting, currentScript]() { scripting->RemoveProjectScript(currentScript); }
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
        std::string scriptName = currentScript->getName();

        // Configurar CustomTreeNode para cada script
        CustomImGui::CustomTreeNodeConfig scriptConfig(scriptName.c_str());
        scriptConfig.leftIcon = FileSystemGUI::Icon(IconGUI::SCRIPT_FILE);
        scriptConfig.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        scriptConfig.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
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
            [scripting, currentScript]() { scripting->RemoveSceneScript(currentScript); }
        );

        bool isOpenCurrentScript = CustomImGui::CustomTreeNode(scriptConfig, nullptr);

        if (isOpenCurrentScript) {
            currentScript->DrawImGuiProperties();
            CustomImGui::CustomTreePop();
        }

        ImGui::PopID();
    }
}

void GUIAddonProjectSetup::DrawSceneCustomShaders()
{
    auto render = Components::get()->Render();
    auto shaders = render->getSceneShaders();

    for (int i = 0; i < shaders.size(); i++) {
        auto currentShader = shaders[i];
        ImGui::PushID(i);

        // Evaluar estados ANTES de crear el config
        bool isEnabled = currentShader->isEnabled();
        std::string shaderLabel = currentShader->getLabel();

        // Configurar CustomTreeNode para cada shader
        CustomImGui::CustomTreeNodeConfig shaderConfig(shaderLabel.c_str());
        shaderConfig.leftIcon = FileSystemGUI::Icon(IconGUI::SHADER_FILE);
        shaderConfig.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        shaderConfig.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
        shaderConfig.defaultOpen = false;

        // Botones de acción
        shaderConfig.actionItems.emplace_back(
            !isEnabled ? FileSystemGUI::Icon(IconGUI::SHADER_LOCK) : FileSystemGUI::Icon(IconGUI::SHADER_UNLOCK),
            !isEnabled ? "Unlock scene shader" : "Lock scene shader",
            [currentShader]() { currentShader->setEnabled(!currentShader->isEnabled()); }
        );

        shaderConfig.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SHADER_EDIT),
            "Edit shader",
            [currentShader]() { ShadersGUI::LoadDialogShader(currentShader->getTypesFile()); }
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
            [render, index]() { render->RemoveSceneShaderByIndex(index); }
        );

        bool isOpenCurrentShader = CustomImGui::CustomTreeNode(shaderConfig, nullptr);

        if (isOpenCurrentShader) {
            currentShader->DrawImGuiProperties(nullptr, nullptr);
            CustomImGui::CustomTreePop();
        }

        ImGui::PopID();
    }
}