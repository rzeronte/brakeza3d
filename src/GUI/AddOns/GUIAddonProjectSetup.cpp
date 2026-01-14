//
// Created by Eduardo on 06/12/2025.
//

#include "../include/Brakeza.h"
#include "../../../include/Components/Components.h"
#include "../../../include/GUI/AddOns/GUIAddonProjectSetup.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/GUI/Objects/ShadersGUI.h"
#include "../../../include/Render/Drawable.h"

void GUIAddonProjectSetup::TreeSceneScripts()
{
    auto scripting = Components::get()->Scripting();
    static bool shouldOpen = false;

    std::string labelSceneScripts = "Scene scripts (" + std::to_string(scripting->getSceneLUAScripts().size()) + ")";
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
        if (scripting->getSceneLUAScripts().empty()) {
            Drawable::WarningMessage("There are not shaders attached");
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

void GUIAddonProjectSetup::TreeProjectScripts()
{
    auto scripting = Components::get()->Scripting();
    static bool shouldOpen = false;

    std::string labelGlobalScripts = "Project scripts (" + std::to_string(scripting->getProjectLUAScripts().size()) + ")";
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
        if (scripting->getProjectLUAScripts().empty()) {
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
            Config::DragDropCustomShaderData* receivedData = (Config::DragDropCustomShaderData*)payload->Data;
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

void GUIAddonProjectSetup::DrawWinProjectSettings()
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::PROJECT_SETTINGS);
    if (!windowStatus->isOpen) return;

    static char name[256];
    strncpy(name, Config::get()->ENGINE_TITLE.c_str(), sizeof(name));
    ImGui::InputText("##Project title", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll);
    if (ImGui::IsItemEdited()) {
        Config::get()->ENGINE_TITLE = name;
    }
    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 4.0f));
    TreeProjectScripts();
    TreeProjectScenes();
    ImGui::Separator();
    TreeSceneScripts();
    TreeSceneShaders();
    ImGui::PopStyleVar();
}

void GUIAddonProjectSetup::DrawProjectScenes()
{
    auto scripting = Components::get()->Scripting();

    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    auto scenes = scripting->getProjectScenes();

    if (ImGui::BeginTable("ProjectScenesTable", 2, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Scene");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        for (unsigned int i = 0; i < (unsigned int) scenes.size(); i++) {
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
            });
            ImGui::SameLine();
            GUI::DrawButtonTransparent("Remove scene from project", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            });

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void GUIAddonProjectSetup::DrawProjectScripts()
{
    auto scripting = Components::get()->Scripting();

    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    auto scripts = scripting->getProjectLUAScripts();

    if (ImGui::BeginTable("ProjectScriptsTable", 2, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Script");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed);
        for (unsigned int i = 0; i < (unsigned int) scripts.size(); i++) {
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
    auto scripts = scripting->getSceneLUAScripts();

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

        for (unsigned int i = 0; i < shaders.size(); i++) {
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