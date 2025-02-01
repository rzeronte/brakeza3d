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
            auto camera = ComponentsManager::get()->getComponentCamera();
            Drawable::drawObject3DGizmo(
                o,
                operation,
                o->getModelMatrix(),
                camera->getGLMMat4ViewMatrix(),
                camera->getGLMMat4ProjectionMatrix()
            );
        }

        if (ImGui::Begin("Object Properties")) {
            if (hasSelectedIndex) {
                auto o = gameObjects[selectedObjectIndex];
                if (o->isRemoved()) {
                    return;
                }

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
