//
// Created by Eduardo on 06/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonProjectSetup.h"
#include "../../../include/Components/Components.h"
#include "../include/Brakeza.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"

GUIAddonProjectSetup::GUIAddonProjectSetup()
{
}

void GUIAddonProjectSetup::DrawWinProjectSettings(GUIManager *gui)
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
    if (ImGui::TreeNodeEx("Project content", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen)) {
        auto scripting = Components::get()->Scripting();
        auto render = Components::get()->Render();

        std::string labelGlobalScripts = "Global scripts (" + std::to_string(scripting->getProjectLUAScripts().size()) + ")";
        ImGui::Image(FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_GLOBAL_SCRIPTS), GUIType::Sizes::ICONS_TOOLBAR); ImGui::SameLine();
        if (ImGui::TreeNodeEx(labelGlobalScripts.c_str(), ImGuiTreeNodeFlags_FramePadding| ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawProjectScripts(gui);
            ImGui::TreePop();
        }
        std::string labelSceneScripts = "Scene scripts (" + std::to_string(scripting->getSceneLUAScripts().size()) + ")";
        ImGui::Image(FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENE_SCRIPTS), GUIType::Sizes::ICONS_TOOLBAR); ImGui::SameLine();
        if (ImGui::TreeNodeEx(labelSceneScripts.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawSceneScripts(gui);
            ImGui::TreePop();
        }
        std::string labelSceneShaders = "Scene shaders (" + std::to_string(render->getSceneShaders().size()) + ")";
        ImGui::Image(FileSystemGUI::Icon(IconGUI::PROJECT_SETUP_SCENE_SHADERS), GUIType::Sizes::ICONS_TOOLBAR); ImGui::SameLine();
        if (ImGui::TreeNodeEx(labelSceneShaders.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen)) {
            DrawSceneCustomShaders(gui);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::PopStyleVar();
}

void GUIAddonProjectSetup::DrawProjectScripts(GUIManager *gui)
{
    auto scripting = Components::get()->Scripting();
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
            Logging::Message("Dropping script (%s) in global space", payload->Data);
            scripting->addProjectLUAScript(new ScriptLUA(
                std::string((char *) payload->Data),
                std::string((char *) payload->Data),
                ScriptLUA::dataTypesFileFor(std::string((char *) payload->Data)))
            );
        }
        ImGui::EndDragDropTarget();
    }
    auto scripts = scripting->getProjectLUAScripts();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    for (int i = 0; i < (int) scripts.size(); i++) {
        auto currentScript = scripts[i];
        ImGui::PushID(i);
        GUI::DrawButtonTransparent(
            currentScript->isPaused() ? "Unlock project script##" : "Lock project script##",
            currentScript->isPaused() ? IconGUI::LUA_LOCK : IconGUI::LUA_UNLOCK,
            GUIType::Sizes::ICONS_BROWSERS,
            false,
            [&] { currentScript->setPaused(!currentScript->isPaused());
        });
        ImGui::SameLine();
        GUI::DrawButton("Reload scene script", IconGUI::SCRIPT_RELOAD, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            currentScript->Reload();
        });
        ImGui::SameLine();
        GUI::DrawButton("Remove script", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            scripting->RemoveProjectScript(currentScript);
        });
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
        ImGui::Text((std::to_string(i + 1) + ") " + currentScript->getName()).c_str());

        ImGui::PopID();
    }
}

void GUIAddonProjectSetup::DrawSceneScripts(GUIManager *gui)
{
    auto scripting = Components::get()->Scripting();
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
            Logging::Message("Dropping script (%s) in global space", payload->Data);
            scripting->addSceneLUAScript(
                new ScriptLUA(
                    std::string((char *) payload->Data),
                    std::string((char *) payload->Data),
                        ScriptLUA::dataTypesFileFor(std::string((char *) payload->Data))
                )
            );
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    auto scripts = scripting->getSceneLUAScripts();

    for (unsigned int i = 0; i < scripts.size(); i++) {
        auto currentScript = scripts[i];
        ImGui::PushID(i);
        std::string optionText = std::to_string(i + 1) + ") " + currentScript->getName();

        GUI::DrawButtonTransparent(
            currentScript->isPaused() ? "Unlock scene script" : "Lock scene script",
            currentScript->isPaused() ? IconGUI::LUA_UNLOCK : IconGUI::LUA_UNLOCK,
            GUIType::Sizes::ICONS_BROWSERS,
            false,
            [&] { currentScript->setPaused(!currentScript->isPaused());
        });
        ImGui::SameLine();
        GUI::DrawButton("Reload scene script", IconGUI::SCRIPT_RELOAD, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            currentScript->Reload();
        });
        ImGui::SameLine();
        GUI::DrawButton("Remove scene script", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            scripting->RemoveSceneScript(currentScript);
        });
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(currentScript->getName().c_str(), ImGuiTreeNodeFlags_None)) {
            currentScript->drawImGuiProperties();
        }
        ImGui::PopID();
    }
}

void GUIAddonProjectSetup::DrawSceneCustomShaders(GUIManager *gui)
{
    auto render = Components::get()->Render();

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CUSTOMSHADER_ITEM")) {
            Config::DragDropCustomShaderData* receivedData = (Config::DragDropCustomShaderData*)payload->Data;
            Logging::Message("Dropping shader (Folder: %s, File: %s) in global space", receivedData->folder, receivedData->file);
            render->LoadShaderIntoScene(receivedData->folder, receivedData->file);
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    auto shaders = render->getSceneShaders();
    for (unsigned int i = 0; i < (unsigned int) shaders.size(); i++) {
        auto s = shaders[i];
        ImGui::PushID(i);
        GUI::DrawButtonTransparent(
            !s->isEnabled() ? "UnLock scene shader" : "Lock scene shader",
            !s->isEnabled() ? IconGUI::SHADER_LOCK : IconGUI::SHADER_UNLOCK,
            GUIType::Sizes::ICONS_BROWSERS,
            false,
            [&] { s->setEnabled(!s->isEnabled());}
        );
        ImGui::SameLine();
        GUI::DrawButton("Reload script", IconGUI::LUA_RELOAD, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            s->Reload();
        });
        ImGui::SameLine();
        GUI::DrawButton("Remove script", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            render->RemoveSceneShaderByIndex(i);
        });
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(s->getLabel().c_str(), ImGuiTreeNodeFlags_None)) {
            s->drawImGuiProperties(nullptr, nullptr);
        }
        ImGui::PopID();
    }
}