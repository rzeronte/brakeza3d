//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
#define BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H

#include "imgui.h"
#include "../../3D/Object3D.h"

struct ScriptEditableManager {
    std::string selectedScriptFilename;
    ScriptLUA *script = nullptr;
    char editableSource[1024 * 16];
};

struct ShaderEditableManager {
    bool loaded = false;
    std::string folder;
    std::string name;
    ShaderOGLCustom *shader = nullptr;
    char editableSourceVS[1024 * 16];
    char editableSourceFS[1024 * 16];
};

struct GUIAddonObject3DProperties
{
    std::vector<Object3D *> &gameObjects;
    ScriptEditableManager &scriptEditableManager;

    GUIAddonObject3DProperties(std::vector<Object3D *> &gameObjects, ScriptEditableManager &scriptEditableManager)
    :
        gameObjects(gameObjects),
        scriptEditableManager(scriptEditableManager)
    {
    }

    void Draw(const int &selectedObjectIndex) const
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
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No object selected");
            }
        }
        ImGui::End();
    }
};

#endif //BRAKEZA3D_GUIWIDGETOBJECT3DPROPERTIES_H
