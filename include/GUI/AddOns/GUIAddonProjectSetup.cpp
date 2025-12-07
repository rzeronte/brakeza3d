//
// Created by Eduardo on 06/12/2025.
//

#include "GUIAddonProjectSetup.h"

#include "../../Components/ComponentsManager.h"
#include "../include/Brakeza.h"
#include "../Objects/FileSystemGUI.h"

GUIAddonProjectSetup::GUIAddonProjectSetup()
{
}

void GUIAddonProjectSetup::DrawProjectSetupGUI(GUIManager *gui)
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
    DrawProjectScripts(gui);
    ImGui::Separator();
    DrawSceneScripts(gui);
    ImGui::Separator();
    DrawSceneCustomShaders(gui);
}

void GUIAddonProjectSetup::DrawProjectScripts(GUIManager *gui)
{
    auto scripting = ComponentsManager::get()->getComponentScripting();

    auto scripts = scripting->getProjectLUAScripts();
    ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::DROP_ACCEPT), GUIType::Sizes::DRAG_ACCEPT);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
            Logging::Message("Dropping script (%s) in global space", payload->Data);
            scripting->addProjectLUAScript(new ScriptLUA(
                std::string((char *) payload->Data),
                ScriptLUA::dataTypesFileFor(std::string((char *) payload->Data)))
            );
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    ImGui::Text("Project LUA Scripts");
    ImGui::Separator();
    if (scripts.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Project scripts not found");
    }

    for (int i = 0; i < (int) scripts.size(); i++) {
        auto currentScript = scripts[i];
        ImGui::PushID(i);
        std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;
        GUI::DrawButton("Remove script", IconGUI::REMOVE, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
            scripting->removeProjectScript(currentScript);
        });
        ImGui::SameLine();
        if (currentScript->isPaused()) {
            GUI::DrawButton("Unlock script", IconGUI::LUA_UNLOCK, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                currentScript->setPaused(false);
            });

        } else {
            GUI::DrawButton("Lock script", IconGUI::LUA_UNLOCK, GUIType::Sizes::ICONS_BROWSERS, false, [&] {
                currentScript->setPaused(true);
            });

        }
        ImGui::SameLine();
        GUI::DrawButton("Edit script", IconGUI::SCRIPT_EDIT, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            gui->scriptEditableManager.selectedScriptFilename = currentScript->scriptFilename;
            delete gui->scriptEditableManager.script;
            gui->scriptEditableManager.script = new ScriptLUA(gui->scriptEditableManager.selectedScriptFilename, ScriptLUA::dataTypesFileFor(
                    gui->scriptEditableManager.selectedScriptFilename));
            strcpy(gui->scriptEditableManager.editableSource, gui->scriptEditableManager.script->content.c_str());
        });
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
        ImGui::Text((std::to_string(i + 1) + ") " + currentScript->scriptFilename).c_str());

        ImGui::PopID();
    }
}

void GUIAddonProjectSetup::DrawSceneScripts(GUIManager *gui)
{
    auto scripting = ComponentsManager::get()->getComponentScripting();

    ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::DROP_ACCEPT), GUIType::Sizes::DRAG_ACCEPT);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
            Logging::Message("Dropping script (%s) in global space", payload->Data);
            scripting->addSceneLUAScript(new ScriptLUA(
                std::string((char *) payload->Data),
                ScriptLUA::dataTypesFileFor(std::string((char *) payload->Data)))
            );
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    ImGui::Text("Scene LUA Scripts");
    ImGui::Separator();
    auto scripts = scripting->getSceneLUAScripts();
    if (scripts.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Scene scripts not found");
    }

    for (unsigned int i = 0; i < scripts.size(); i++) {
        auto currentScript = scripts[i];
        ImGui::PushID(i);
        std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;

        GUI::DrawButton("Remove script", IconGUI::REMOVE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            scripting->removeSceneScript(currentScript);
        });

        ImGui::SameLine();
        if (currentScript->isPaused()) {
            GUI::DrawButton("UnLock Script", IconGUI::LUA_UNLOCK, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                currentScript->setPaused(false);
            });
        } else {
            GUI::DrawButton("Lock Script", IconGUI::LUA_LOCK, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                currentScript->setPaused(true);
            });
        }
        ImGui::SameLine();
        GUI::DrawButton("Edit scene script", IconGUI::SCRIPT_EDIT, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            gui->scriptEditableManager.selectedScriptFilename = currentScript->scriptFilename;
            delete gui->scriptEditableManager.script;
            gui->scriptEditableManager.script = new ScriptLUA(gui->scriptEditableManager.selectedScriptFilename, ScriptLUA::dataTypesFileFor(
                    gui->scriptEditableManager.selectedScriptFilename));
            strcpy(gui->scriptEditableManager.editableSource, gui->scriptEditableManager.script->content.c_str());
        });
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
        ImGui::Text((std::to_string(i + 1) + ") " + currentScript->scriptFilename).c_str());

        ImGui::PopID();
    }
}

void GUIAddonProjectSetup::DrawSceneCustomShaders(GUIManager *gui)
{
    ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::DROP_ACCEPT), GUIType::Sizes::DRAG_ACCEPT);
    auto render = ComponentsManager::get()->getComponentRender();
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CUSTOMSHADER_ITEM")) {
            Config::DragDropCustomShaderData* receivedData = (Config::DragDropCustomShaderData*)payload->Data;
            Logging::Message("Dropping shader (Folder: %s, File: %s) in global space", receivedData->folder, receivedData->file);
            render->loadShaderIntoScene(receivedData->folder, receivedData->file);
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
    ImGui::Text("Scene Shaders");
    ImGui::Separator();
    auto shaders = render->getSceneShaders();
    if (shaders.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Scene shaders not found");
    }
    for (unsigned int i = 0; i < (unsigned int) shaders.size(); i++) {
        auto s = shaders[i];
        ImGui::PushID(i);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_FILE), ImVec2(26, 26));
        ImGui::SameLine(46);
        if (!s->isEnabled()) {
            GUI::DrawButton("UnLock Script", IconGUI::LUA_UNLOCK, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                s->setEnabled(true);
            });
        } else {
            GUI::DrawButton("Lock Script", IconGUI::LUA_LOCK, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
                s->setEnabled(false);
            });
        }
        ImGui::SameLine();
        GUI::DrawButton("Reload script", IconGUI::LUA_RELOAD, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            s->reload();
        });
        ImGui::SameLine();
        GUI::DrawButton("Remove script", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_BROWSERS, true, [&] {
            render->removeSceneShaderByIndex(i);
        });
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(s->getLabel().c_str(), ImGuiTreeNodeFlags_None)) {
            ImGui::PushID(i);
            s->drawImGuiProperties(nullptr, nullptr);
            ImGui::PopID();
        }
        ImGui::PopID();
    }
}