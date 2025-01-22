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

    void draw(int &selectedObjectIndex, ImGuizmo::OPERATION &operation)
    {
        bool hasSelectedIndex = selectedObjectIndex >= 0 && selectedObjectIndex < gameObjects.size();

        if (hasSelectedIndex) {
            auto o = gameObjects[selectedObjectIndex];
            Drawable::drawObject3DGizmo(
                o,
                operation,
                o->getModelMatrix(),
                ComponentsManager::get()->getComponentCamera()->getCamera()->getViewMatrix(),
                Camera3D::getProjectionMatrix()
            );
        }

        if (ImGui::Begin("Properties")) {
            if (hasSelectedIndex) {
                auto o = gameObjects[selectedObjectIndex];
                if (o->isRemoved()) {
                    return;
                }

                ImGui::Text("Tools:");
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "translateIcon"), ImVec2(32, 32))) {
                    operation = ImGuizmo::OPERATION::TRANSLATE;
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "rotateIcon"), ImVec2(32, 32))) {
                    operation = ImGuizmo::OPERATION::ROTATE;
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "scaleIcon"), ImVec2(32, 32))) {
                    operation = ImGuizmo::OPERATION::SCALE_X;
                }
                ImGui::Separator();
                o->drawImGuiProperties();

                ImGui::Separator();

                if (ImGui::TreeNode("Scripts LUA")) {
                    auto objectScripts = o->getScripts();

                    if ((int) objectScripts.size() <= 0) {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Not Scripts LUA found");
                    }

                    for (int i = 0; i < (int) objectScripts.size(); i++) {
                        auto currentScript = objectScripts[i];
                        ImGui::PushID(i);

                        std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;

                        ImGui::Image(TexturePackage::getOGLTextureID(ImGuiTextures, "scriptIcon"), ImVec2(24, 24));
                        ImGui::SameLine();

                        if (ImGui::Button(optionText.c_str())) {
                            delete scriptEditableManager.script;
                            scriptEditableManager.selectedScriptFilename = currentScript->scriptFilename;
                            scriptEditableManager.script = new ScriptLUA(
                                    scriptEditableManager.selectedScriptFilename,
                                    ScriptLUA::dataTypesFileFor(scriptEditableManager.selectedScriptFilename)
                            );
                            strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content.c_str());
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
                        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "removeIcon"), ImVec2(14, 14))) {
                            o->removeScript(currentScript);
                        }

                        currentScript->drawImGuiProperties();

                        ImGui::Separator();

                        ImGui::PopID();
                    }
                    ImGui::TreePop();
                }

                ImGui::Separator();

                ImGui::Button("Remove");
                if (ImGui::IsItemClicked()) {
                    o->setRemoved(true);
                }
                ImGui::SameLine();
            } else {
                ImGui::Text("Select an object...");
            }
        }
        ImGui::End();
    }
};

#endif //BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
