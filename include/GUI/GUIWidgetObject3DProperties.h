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
        bool hasSelectedIndex = selectedObjectIndex >= 0 && selectedObjectIndex < gameObjects.size();

        if (ImGui::Begin("Object Properties")) {
            if (hasSelectedIndex) {
                auto o = gameObjects[selectedObjectIndex];
                if (o->isRemoved()) {
                    return;
                }

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
