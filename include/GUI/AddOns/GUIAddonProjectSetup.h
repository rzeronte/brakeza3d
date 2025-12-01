//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
#define BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H

#include "imgui.h"
#include "../../Components/ComponentsManager.h"
#include "GUIAddonObject3DProperties.h"
#include "../Objects/FileSystemGUI.h"

struct GUIAddonProjectSetup
{
    ScriptEditableManager &scriptEditableManager;

    GUIAddonProjectSetup(ScriptEditableManager &scriptEditableManager)
    :
        scriptEditableManager(scriptEditableManager)
    {
    }

    void DrawProjectSetupGUI()
    {
        static char name[256];

        strncpy(name, BrakezaSetup::get()->ENGINE_TITLE.c_str(), sizeof(name));

        ImGui::InputText("Project title##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::IsItemEdited()) {
            BrakezaSetup::get()->ENGINE_TITLE = name;
        }
        ImGui::Separator();
        DrawProjectScripts();
        ImGui::Separator();
        DrawSceneScripts();
        ImGui::Separator();
        DrawSceneCustomShaders();

    }

    void DrawProjectScripts() const
    {
        auto scripting = ComponentsManager::get()->getComponentScripting();

        auto scripts = scripting->getProjectLUAScripts();
        ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::ADD), ImVec2(16, 16));
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
            if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::REMOVE), ImVec2(14, 14))) {
                scripting->removeProjectScript(currentScript);
            }
            ImGui::SameLine();
            if (currentScript->isPaused()) {
                if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::UNLOCK), ImVec2(14, 14))) {
                    currentScript->setPaused(false);
                }
            } else {
                if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::LOCK), ImVec2(14, 14))) {
                    currentScript->setPaused(true);
                }
            }
            ImGui::SameLine();
            if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::SCRIPT), ImVec2(14, 14))) {
                scriptEditableManager.selectedScriptFilename = currentScript->scriptFilename;
                delete scriptEditableManager.script;
                scriptEditableManager.script = new ScriptLUA(scriptEditableManager.selectedScriptFilename, ScriptLUA::dataTypesFileFor(
                        scriptEditableManager.selectedScriptFilename));
                strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content.c_str());
            }
            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text((std::to_string(i + 1) + ") " + currentScript->scriptFilename).c_str());

            ImGui::PopID();
        }
    }

    void DrawSceneScripts() const
    {
        auto scripting = ComponentsManager::get()->getComponentScripting();

        ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::ADD), ImVec2(16, 16));
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

        for (int i = 0; i < scripts.size(); i++) {
            auto currentScript = scripts[i];
            ImGui::PushID(i);
            std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;
            if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::REMOVE), ImVec2(14, 14))) {
                scripting->removeSceneScript(currentScript);
            }
            ImGui::SameLine();
            if (currentScript->isPaused()) {
                if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::UNLOCK), ImVec2(14, 14))) {
                    currentScript->setPaused(false);
                }
            } else {
                if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::LOCK), ImVec2(14, 14))) {
                    currentScript->setPaused(true);
                }
            }
            ImGui::SameLine();
            if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::SCRIPT), ImVec2(14, 14))) {
                scriptEditableManager.selectedScriptFilename = currentScript->scriptFilename;
                delete scriptEditableManager.script;
                scriptEditableManager.script = new ScriptLUA(scriptEditableManager.selectedScriptFilename, ScriptLUA::dataTypesFileFor(
                        scriptEditableManager.selectedScriptFilename));
                strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content.c_str());
            }
            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text((std::to_string(i + 1) + ") " + currentScript->scriptFilename).c_str());

            ImGui::PopID();
        }
    }

    static void DrawSceneCustomShaders()
    {
        ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::ADD), ImVec2(16, 16));
        auto render = ComponentsManager::get()->getComponentRender();
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CUSTOMSHADER_ITEM")) {
                BrakezaSetup::DragDropCustomShaderData* receivedData = (BrakezaSetup::DragDropCustomShaderData*)payload->Data;
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
        for (int i = 0; i < shaders.size(); i++) {
            auto s = shaders[i];
            ImGui::PushID(i);
            ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::SHADER), ImVec2(26, 26));
            ImGui::SameLine(46);
            if (!s->isEnabled()) {
                if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::UNLOCK), ImVec2(14, 14))) {
                    s->setEnabled(true);
                }
            } else {
                if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::LOCK), ImVec2(14, 14))) {
                    s->setEnabled(false);
                }
            }
            ImGui::SameLine();
            if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::REBUILD), ImVec2(14, 14))) {
                s->reload();
            }
            ImGui::SameLine();
            if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::REMOVE), ImVec2(14, 14))) {
                render->removeSceneShaderByIndex(i);
            }
            ImGui::SameLine();
            if (ImGui::CollapsingHeader(s->getLabel().c_str(), ImGuiTreeNodeFlags_None)) {
                ImGui::PushID(i);
                s->drawImGuiProperties(nullptr, nullptr);
                ImGui::PopID();
            }
            ImGui::PopID();
        }
    }
};

#endif //BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
