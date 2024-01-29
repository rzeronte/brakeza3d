//
// Created by edu on 29/12/23.
//

#include <string>
#include "imgui.h"
#include "../Objects/Object3D.h"
#include "../Render/Logging.h"
#include "../FXEffect/FXShockWave.h"
#include "../Objects/Mesh3D.h"
#include "../FXEffect/FXOutliner.h"
#include "../FXEffect/FXBlink.h"
#include "../Misc/TexturePackage.h"

#ifndef BRAKEZA3D_GUIOBJECTPROPERTIES_H
#define BRAKEZA3D_GUIOBJECTPROPERTIES_H

#endif //BRAKEZA3D_GUIOBJECTPROPERTIES_H


struct GUIWidgetObjects3D {
    TexturePackage &ImGuiTextures;
    std::vector<Object3D *> &gameObjects;

    GUIWidgetObjects3D(TexturePackage &imGuiTextures, std::vector<Object3D *> &gameObjects)
    :
        ImGuiTextures(imGuiTextures),
        gameObjects(gameObjects)
    {
    }

    void draw(int &selectedObjectIndex)
    {
        if (ImGui::Begin("Scene Objects")) {
            auto title = std::string("Number objects: ") + std::to_string(gameObjects.size());

            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", title.c_str());

            ImGui::Separator();

            for (int i = 0; i < gameObjects.size(); i++) {
                auto o = gameObjects[i];

                if (o->isRemoved()) {
                    continue;
                }

                auto projectile = dynamic_cast<Projectile3DBody*> (o);
                if (projectile != nullptr) continue;

                std::string optionText = std::to_string(i + 1) + ") " + o->getLabel();
                if (ImGui::Selectable(optionText.c_str(), selectedObjectIndex == i)) {
                    selectedObjectIndex = i;
                }
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
                        Logging::Message("Dropping script (%s) in %s", payload->Data, o->getLabel().c_str());
                        o->attachScript(new ScriptLUA(
                                std::string((const char*) payload->Data),
                                ScriptLUA::dataTypesFileFor(std::string((char *)payload->Data)))
                        );
                    }
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH3D_SHADER_ITEM")) {
                        Logging::Message("Dropping shader (%s) in %s", payload->Data, o->getLabel().c_str());
                        int selection = std::stoi((char*) payload->Data);
                        switch(selection) {
                            case 2: {
                                auto shader = new FXShockWave(true, o, 1.5f, ShockWaveParams::standard());
                                o->addMesh3DShader(shader);
                                break;
                            }
                            case 3: {
                                auto shader = new FXColorTint(true, Color::red(), 0.0f);
                                o->addMesh3DShader(shader);
                                break;
                            }
                        }
                        auto mesh = dynamic_cast<Mesh3D*> (o);
                        if (mesh) {
                            switch(selection) {
                                case 0: {
                                    auto shader = new FXOutliner(true, mesh, Color::green(), 1);
                                    mesh->addMesh3DShader(shader);
                                    break;
                                }
                                case 1: {
                                    auto shader = new FXBlink(true, mesh, 0.1f , Color::green());
                                    mesh->addMesh3DShader(shader);
                                    break;
                                }

                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SameLine();
                if (o->isEnabled()) {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(14, 14));
                } else {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(14, 14));
                }
                ImGui::SameLine();

                ImGui::SameLine(250);
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel(o->getTypeIcon())->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine(270);
                ImGui::Text("%s", o->getTypeObject());
            }
        }
        ImGui::End();
    }
};