
#ifndef BRAKEDA3D_GUI_CAMERA_H
#define BRAKEDA3D_GUI_CAMERA_H


#include "GUI.h"
#include "../Components/Camera3D.h"
#include "../../imgui/imgui.h"
#include "../EngineSetup.h"
#include "../ComponentsManager.h"

class GUI_Camera : public GUI {
public:
    bool show = false;

    virtual void draw(Camera3D *camera) {
        ImGuiWindowFlags window_flags = 0;

        if (show) {

            ImGui::SetNextWindowPos(ImVec2(250, 22), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 190), ImGuiSetCond_Once);
            //window_flags |= ImGuiWindowFlags_NoMove;

            std::string title = "Camera Inspector";
            ImGui::Begin(title.c_str(), &show, window_flags);

            const float range_min = 0;
            const float range_max = 1000;

            const float range_sensibility = -0.1;
            const float range_min_yaw = 0;
            const float range_max_yaw = 0;

            const float range_min_movement = 0;
            const float range_max_movement = 500;

            const float range_min_mouse_sensitivity = 0;
            const float range_max_mouse_sensitivity = 3;


            std::string position_text = "Position##1";
            std::string rotation_text = "Orientation##2";
            std::string movement_text = "Keyboard##3";
            std::string mouse_text = "Mouse##4";
            std::string autoscrollSpeed_text = "AutoScroll##5";

            // position
            if (ImGui::TreeNode(position_text.c_str())) {
                ImGui::DragScalar("X", ImGuiDataType_Float, &camera->getPosition().x, range_sensibility, &range_min,&range_max, "%f", 1.0f);
                ImGui::DragScalar("Y", ImGuiDataType_Float, &camera->getPosition().y, range_sensibility, &range_min,&range_max, "%f", 1.0f);
                ImGui::DragScalar("Z", ImGuiDataType_Float, &camera->getPosition().z, range_sensibility, &range_min,&range_max, "%f", 1.0f);
                ImGui::TreePop();
            }

            ImGui::Separator();

            // rotation
            if (ImGui::TreeNode(rotation_text.c_str())) {
                ImGui::DragScalar("Yaw", ImGuiDataType_Float, &camera->yaw, range_sensibility, &range_min_yaw,&range_max_yaw, "%f", 1.0f);
                ImGui::DragScalar("Pitch", ImGuiDataType_Float, &camera->pitch, range_sensibility, &range_min_yaw,&range_max_yaw, "%f", 1.0f);
                ImGui::DragScalar("Roll", ImGuiDataType_Float, &camera->roll, range_sensibility, &range_min_yaw,&range_max_yaw, "%f", 1.0f);
                ImGui::TreePop();
            }

            ImGui::Separator();

            if (ImGui::TreeNode(movement_text.c_str())) {
                ImGui::DragScalar("Walking", ImGuiDataType_Float, &EngineSetup::get()->WALKING_SPEED,range_sensibility, &range_min_movement, &range_max_movement, "%f", 1.0f);
                ImGui::DragScalar("Turn", ImGuiDataType_Float, &EngineSetup::get()->TURN_SPEED,range_sensibility, &range_min_movement, &range_max_movement, "%f", 1.0f);
                ImGui::DragScalar("Pitch", ImGuiDataType_Float, &EngineSetup::get()->PITCH_SPEED,range_sensibility, &range_min_movement, &range_max_movement, "%f", 1.0f);
                ImGui::DragScalar("Strafe", ImGuiDataType_Float, &EngineSetup::get()->STRAFE_SPEED,range_sensibility, &range_min_movement, &range_max_movement, "%f", 1.0f);
                ImGui::TreePop();
            }
            ImGui::Separator();

            if (ImGui::TreeNode(std::string("Velocity").c_str())) {
                ImGui::Text((std::string("X: ") + std::to_string(camera->velocity.getComponent().x)).c_str());
                ImGui::Text((std::string("Y: ") + std::to_string(camera->velocity.getComponent().y)).c_str());
                ImGui::Text((std::string("Z: ") + std::to_string(camera->velocity.getComponent().z)).c_str());
                ImGui::TreePop();
            }
            ImGui::Separator();

            if (ImGui::TreeNode(mouse_text.c_str())) {
                ImGui::DragScalar("Sens.", ImGuiDataType_Float, &EngineSetup::get()->MOUSE_SENSITIVITY,range_sensibility, &range_min_mouse_sensitivity, &range_max_mouse_sensitivity, "%f",1.0f);
                ImGui::TreePop();
            }

            ImGui::Separator();

            if (ImGui::TreeNode(autoscrollSpeed_text.c_str())) {
                ImGui::Checkbox("AutoScroll Enabled", &ComponentsManager::get()->getComponentCamera()->autoScroll);
                const float range_autoscroll_sensibility = 0.1;
                const float range_autoscroll_min = 0;
                const float range_autoscroll_max = 0;
                ImGui::DragScalar("Speed X", ImGuiDataType_Float, &ComponentsManager::get()->getComponentCamera()->autoScrollSpeed.x,range_autoscroll_sensibility, &range_autoscroll_min, &range_autoscroll_max, "%f", 1.0f);
                ImGui::DragScalar("Speed Y", ImGuiDataType_Float, &ComponentsManager::get()->getComponentCamera()->autoScrollSpeed.y,range_autoscroll_sensibility, &range_autoscroll_min, &range_autoscroll_max, "%f", 1.0f);
                ImGui::DragScalar("Speed Z", ImGuiDataType_Float, &ComponentsManager::get()->getComponentCamera()->autoScrollSpeed.z,range_autoscroll_sensibility, &range_autoscroll_min, &range_autoscroll_max, "%f", 1.0f);
                ImGui::TreePop();
            }
            ImGui::Separator();
            ImGui::End();
        }
    }
};


#endif //BRAKEDA3D_GUI_CAMERA_H