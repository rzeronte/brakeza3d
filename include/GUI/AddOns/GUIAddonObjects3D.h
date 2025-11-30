#ifndef BRAKEZA3D_GUIWIDGETOBJECTS3D_H
#define BRAKEZA3D_GUIWIDGETOBJECTS3D_H

#include <string>
#include "imgui.h"
#include "../../Misc/Logging.h"
#include "../../3D/Mesh3D.h"
#include "../../Components/ComponentsManager.h"
#include "../Objects/FileSystemGUI.h"

struct GUIWidgetAllowedObjectConfig {
    std::string typeObject;
    bool visible;
    const char* icon;
};

struct GUIAddonObjects3D {
    std::vector<Object3D *> &gameObjects;
    std::vector<GUIWidgetAllowedObjectConfig> allowedObjectsToShow;
    
    GUIAddonObjects3D(std::vector<Object3D *> &gameObjects)
    :
        gameObjects(gameObjects)
    {
        allowedObjectsToShow.push_back({SceneObjectTypes::OBJECT_3D, true, IconsByObject::OBJECT_3D});
        allowedObjectsToShow.push_back({SceneObjectTypes::IMAGE_2D, true, IconsByObject::IMAGE_2D});
        allowedObjectsToShow.push_back({SceneObjectTypes::IMAGE_2D_ANIMATION, true, IconsByObject::IMAGE_2D_ANIMATION});
        allowedObjectsToShow.push_back({SceneObjectTypes::MESH_3D, true, IconsByObject::MESH_3D});
        allowedObjectsToShow.push_back({SceneObjectTypes::MESH_3D_ANIMATION, true, IconsByObject::MESH_3D_ANIMATION});
        allowedObjectsToShow.push_back({SceneObjectTypes::IMAGE_3D, true, IconsByObject::IMAGE_3D});
        allowedObjectsToShow.push_back({SceneObjectTypes::IMAGE_3D_ANIMATION, true, IconsByObject::IMAGE_3D_ANIMATION});
        allowedObjectsToShow.push_back({SceneObjectTypes::IMAGE_3D_ANIMATION_8DIR, true, IconsByObject::IMAGE_3D_ANIMATION_8DIR});
        allowedObjectsToShow.push_back({SceneObjectTypes::LIGHT_POINT, true, IconsByObject::LIGHT_POINT});
        allowedObjectsToShow.push_back({SceneObjectTypes::LIGHT_SPOT, true, IconsByObject::LIGHT_SPOT});
        allowedObjectsToShow.push_back({SceneObjectTypes::PARTICLE_EMITTER, true, IconsByObject::PARTICLE_EMITTER});
    }

    void drawAllowedObjectsToShow()
    {
        for (auto& o : allowedObjectsToShow) {
            bool wasVisible = o.visible;

            if (!wasVisible) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); //  rojo
            }
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 7.0f, ImGui::GetCursorPosY()));
            if (ImGui::ImageButton(FileSystemGUI::IconTag( o.icon), ImVec2(14, 14))) {
                o.visible = !o.visible;
            }
            if (!wasVisible) {
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }

    bool isAllowedObjectInConfig(const std::string& typeObject)
    {
        for (const auto& o : allowedObjectsToShow) {
            if (o.visible && o.typeObject == typeObject ) {
                return true;
            }
        }

        return false;
    }

    void Draw(int &selectedObjectIndex)
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

                ImGui::Checkbox(std::string("##"+ std::to_string(o->getId())).c_str(), &o->isEnabled());
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 5.0f, ImGui::GetCursorPosY() + 3.0f));
                ImGui::Image(FileSystemGUI::IconTag(o->getTypeIcon()), ImVec2(16, 16));

                ImGui::SameLine();
                ImGui::SetNextItemWidth(150.0f);
                if (ImGui::Selectable(std::string("##select"+ std::to_string(i)).c_str(), selectedObjectIndex == i)) {
                    selectedObjectIndex = i;
                    ComponentsManager::get()->getComponentRender()->setSelectedObject(gameObjects[selectedObjectIndex]);
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 15.0f);
                ImGui::Text((std::to_string(i + 1) + ") " + o->getLabel()).c_str());
                if (ImGui::BeginDragDropTarget()) {
                    auto mesh = dynamic_cast<Mesh3D*> (o);

                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
                        Logging::Message("Dropping script (%s) in %s", payload->Data, o->getLabel().c_str());
                        o->AttachScript(new ScriptLUA(
                            std::string((const char*) payload->Data),
                            ScriptLUA::dataTypesFileFor(std::string((char *)payload->Data)))
                        );
                    }

                    if (mesh != nullptr) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CUSTOMSHADER_ITEM")) {
                            BrakezaSetup::DragDropCustomShaderData* receivedData = (BrakezaSetup::DragDropCustomShaderData*)payload->Data;
                            Logging::Message("Dropping shader into Mesh3D (Folder: %s, File: %s)", receivedData->folder, receivedData->file);
                            mesh->loadShader(receivedData->folder, receivedData->file);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));
                ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::SCRIPT), ImVec2(14, 14));
                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 10.0f, ImGui::GetCursorPosY()));
                ImGui::Text(std::to_string((int)o->getScripts().size()).c_str());
                if (o->isCollisionsEnabled()) {
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));

                    if (o->getCollisionMode() == GHOST) {
                        ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::GHOST), ImVec2(14, 14));
                    }
                    if (o->getCollisionMode() == BODY) {
                        ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::GEAR), ImVec2(14, 14));
                    }
                    if (o->getCollisionMode() == KINEMATIC) {
                        ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::PLAY), ImVec2(14, 14));
                    }
                }
            }
        }
        ImGui::End();
    }
};

#endif //BRAKEZA3D_GUIWIDGETOBJECTS3D_H
