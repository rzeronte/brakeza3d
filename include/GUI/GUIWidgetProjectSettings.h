//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
#define BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H

#include "imgui.h"
#include "../ComponentsManager.h"
#include "GUIWidgetObject3DProperties.h"

struct GUIWidgetProjectSettings {
    TexturePackage &ImGuiTextures;
    ScriptEditableManager &scriptEditableManager;

    GUIWidgetProjectSettings(TexturePackage &imGuiTextures,ScriptEditableManager &scriptEditableManager)
    :
        ImGuiTextures(imGuiTextures),
        scriptEditableManager(scriptEditableManager)
    {
    }

    void draw()
    {
        if (ImGui::Begin("Project")) {
            auto componentRender = ComponentsManager::get()->getComponentRender();
            static char name[256];

            strncpy(name, EngineSetup::get()->ENGINE_TITLE.c_str(), sizeof(name));

            ImGui::InputText("Scene name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll);
            if (ImGui::IsItemEdited()) {
                EngineSetup::get()->ENGINE_TITLE = name;
            }
            ImGui::Separator();

            auto scripts = componentRender->getLUAScripts();

            ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "addIcon"), ImVec2(16, 16));
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
                    Logging::Message("Dropping script (%s) in global space", payload->Data);
                    componentRender->addLUAScript(new ScriptLUA(
                            std::string((char*) payload->Data),
                            ScriptLUA::dataTypesFileFor(std::string((char *)payload->Data)))
                    );
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
            ImGui::Text("Scene LUA Scripts");
            ImGui::Separator();
            if ((int) scripts.size() <= 0) {
                ImGui::Text("No scripts found");
            }

            for (int i = 0; i < (int) scripts.size(); i++) {
                auto currentScript = scripts[i];
                ImGui::PushID(i);
                std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "removeIcon"), ImVec2(14, 14))) {
                    componentRender->removeScript(currentScript);
                }
                ImGui::SameLine();
                if (currentScript->isPaused()) {
                    if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "unlockIcon"), ImVec2(14, 14))) {
                        currentScript->setPaused(false);
                    }
                } else {
                    if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "lockIcon"), ImVec2(14, 14))) {
                        currentScript->setPaused(true);
                    }
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "scriptIcon"), ImVec2(14, 14))) {
                    scriptEditableManager.selectedScriptFilename = currentScript->scriptFilename;
                    delete scriptEditableManager.script;
                    scriptEditableManager.script = new ScriptLUA(scriptEditableManager.selectedScriptFilename, ScriptLUA::dataTypesFileFor(scriptEditableManager.selectedScriptFilename));
                    strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content.c_str());
                }
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
                ImGui::Text((std::to_string(i + 1) + ") " + currentScript->scriptFilename).c_str());

                ImGui::PopID();
            }

            ImGui::Separator();
        }
        ImGui::End();
    }
};

#endif //BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
