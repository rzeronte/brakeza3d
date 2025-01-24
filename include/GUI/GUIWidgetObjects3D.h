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
#include "../FXEffect/FXColorTint.h"

#ifndef BRAKEZA3D_GUIWIDGETOBJECTS3D_H
#define BRAKEZA3D_GUIWIDGETOBJECTS3D_H

struct GUIWidgetAllowedObjectConfig {
    std::string typeObject;
    bool visible;
    const char* icon;
};

struct GUIWidgetObjects3D {
    TexturePackage &ImGuiTextures;
    std::vector<Object3D *> &gameObjects;
    std::vector<GUIWidgetAllowedObjectConfig> allowedObjectsToShow;
    
    GUIWidgetObjects3D(TexturePackage &imGuiTextures, std::vector<Object3D *> &gameObjects)
    :
        ImGuiTextures(imGuiTextures),
        gameObjects(gameObjects)
    {
        allowedObjectsToShow.push_back({"Object3D", true, "objectIcon"});
        allowedObjectsToShow.push_back({"Image2D", true, "Image2DIcon"});
        allowedObjectsToShow.push_back({"Image2DAnimation", true, "Image2DAnimationIcon"});
        allowedObjectsToShow.push_back({"Mesh3D", true, "meshIcon"});
        allowedObjectsToShow.push_back({"Mesh3DAnimation", true, "Mesh3DAnimationIcon"});
        allowedObjectsToShow.push_back({"Image3D", true, "Image3DIcon"});
        allowedObjectsToShow.push_back({"BillboardAnimation", true, "BillboardAnimationIcon"});
        allowedObjectsToShow.push_back({"BillboardAnimation8Directions", true, "BillboardAnimation8DirectionsIcon"});
        allowedObjectsToShow.push_back({"LightPoint3D", true, "lightIcon"});
        allowedObjectsToShow.push_back({"SpotLight3D", true, "spotLightIcon"});
        allowedObjectsToShow.push_back({"ParticleEmitter", true, "particlesIcon"});
    }

    void drawAllowedObjectsToShow()
    {
        for (auto& o : allowedObjectsToShow) {
            bool wasVisible = o.visible;

            if (!wasVisible) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); //  rojo
            }
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 7.0f, ImGui::GetCursorPosY()));
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, o.icon), ImVec2(14, 14))) {
                o.visible = !o.visible;
            }

            if (!wasVisible) {
                ImGui::PopStyleColor();
            }

            ImGui::SameLine();
        }
        ImGui::NewLine();
    }

    bool isAllowedObjectInConfig(std::string typeObject)
    {
        for (const auto& o : allowedObjectsToShow) {
            if (o.visible && o.typeObject == typeObject ) {
                return true;
            }
        }

        return false;
    }

    void draw(int &selectedObjectIndex)
    {
        if (ImGui::Begin("Scene Objects")) {
            drawAllowedObjectsToShow();
            ImGui::Separator();
            auto title = std::string("Number objects: ") + std::to_string(gameObjects.size());

            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", title.c_str());

            ImGui::Separator();

            for (int i = 0; i < gameObjects.size(); i++) {
                auto o = gameObjects[i];

                if (o->isRemoved()) continue;
                if (!isAllowedObjectInConfig(o->getTypeObject())) continue;

                //auto projectile = dynamic_cast<Projectile3DBody*> (o);
                //if (projectile != nullptr) continue;

                ImGui::Checkbox(std::string("##"+ std::to_string(i)).c_str(), &o->enabled);
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 5.0f, ImGui::GetCursorPosY() + 3.0f));
                ImGui::Image(TexturePackage::getOGLTextureID(ImGuiTextures, o->getTypeIcon()), ImVec2(16, 16));

                ImGui::SameLine();
                ImGui::SetNextItemWidth(150.0f);
                if (ImGui::Selectable(std::string("##select"+ std::to_string(i)).c_str(), selectedObjectIndex == i)) {
                    selectedObjectIndex = i;
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 15.0f);
                ImGui::Text((std::to_string(i + 1) + ") " + o->getLabel()).c_str());
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
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
                ImGui::Image(TexturePackage::getOGLTextureID(ImGuiTextures, "scriptIcon"), ImVec2(14, 14));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 10.0f, ImGui::GetCursorPosY()));
                ImGui::Text(std::to_string((int)o->getScripts().size()).c_str());
                if (o->isCollisionsEnabled()) {
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));

                    if (o->getCollisionMode() == CollisionMode::GHOST) {
                        ImGui::Image(TexturePackage::getOGLTextureID(ImGuiTextures, "ghostIcon"), ImVec2(14, 14));
                    }
                    if (o->getCollisionMode() == CollisionMode::BODY) {
                        ImGui::Image(TexturePackage::getOGLTextureID(ImGuiTextures, "gearIcon"), ImVec2(14, 14));
                    }
                    if (o->getCollisionMode() == CollisionMode::KINEMATIC) {
                        ImGui::Image(TexturePackage::getOGLTextureID(ImGuiTextures, "playerIcon"), ImVec2(14, 14));
                    }
                }
            }
        }
        ImGui::End();
    }
};

#endif //BRAKEZA3D_GUIWIDGETOBJECTS3D_H
