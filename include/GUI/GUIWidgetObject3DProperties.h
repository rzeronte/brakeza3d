//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
#define BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H

#include "imgui.h"
#include "../Misc/TexturePackage.h"
#include "../Objects/Object3D.h"
#include "../Render/Logging.h"
#include "GUIWidgetObject3DProperties.h"

struct ScriptEditableManager {
    std::string selectedScriptFilename;
    ScriptLUA *script = nullptr;
    char editableSource[1024 * 16];
};

struct GUIWidgetObject3DProperties {
    TexturePackage &ImGuiTextures;
    std::vector<Object3D *> &gameObjects;
    ScriptEditableManager &scriptEditableManager;

    GUIWidgetObject3DProperties(
        TexturePackage &imGuiTextures,
        std::vector<Object3D *> &gameObjects,
        ScriptEditableManager &scriptEditableManager
    )
    :
        ImGuiTextures(imGuiTextures),
        gameObjects(gameObjects),
        scriptEditableManager(scriptEditableManager)
    {
    }

    void draw(int &selectedObjectIndex)
    {
        if (ImGui::Begin("Properties")) {
            if (selectedObjectIndex >= 0 && selectedObjectIndex < gameObjects.size()) {
                auto o = gameObjects[selectedObjectIndex];
                if (o->isRemoved()) {
                    return;
                }

                ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Selected object: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", o->getLabel().c_str());
                ImGui::SameLine();
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel(o->getTypeIcon())->getOGLTextureID(), ImVec2(16, 16));

                ImGui::Separator();

                o->drawImGuiProperties();

                ImGui::Separator();

                auto objectScripts = o->getScripts();

                if ((int) objectScripts.size() <= 0) {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No scripts attached");
                }

                for (int i = 0; i < (int) objectScripts.size(); i++) {
                    auto currentScript = objectScripts[i];
                    ImGui::PushID(i);

                    std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;

                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("scriptIcon")->getOGLTextureID(), ImVec2(24, 24));
                    ImGui::SameLine(48);

                    if (ImGui::Button(optionText.c_str())) {
                        delete scriptEditableManager.script;
                        scriptEditableManager.selectedScriptFilename = currentScript->scriptFilename;
                        scriptEditableManager.script = new ScriptLUA(scriptEditableManager.selectedScriptFilename, ScriptLUA::dataTypesFileFor(scriptEditableManager.selectedScriptFilename));
                        strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content);
                    }

                    ImGui::SameLine();

                    if (currentScript->isPaused()) {
                        if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                            currentScript->setPaused(false);
                        }
                    } else {
                        if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                            currentScript->setPaused(true);
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("removeIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                        o->removeScript(currentScript);
                    }

                    currentScript->drawImGuiProperties();

                    ImGui::PopID();
                }

                ImGui::Separator();


                ImGui::Button("Remove");
                if (ImGui::IsItemClicked()) {
                    o->setRemoved(true);
                }
                ImGui::SameLine();

                ImGui::Button("Get JSON!");
                if (ImGui::IsItemClicked()) {
                    Logging::Message("getTypesJSON");
                    std::string json = cJSON_Print(o->getJSON());
                    Logging::Message(json.c_str());
                }

            } else {
                ImGui::Text("Select an object...");
            }
        }
        ImGui::End();
    }
};

#endif //BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
