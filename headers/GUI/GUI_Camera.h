
#ifndef BRAKEDA3D_GUI_CAMERA_H
#define BRAKEDA3D_GUI_CAMERA_H


#include "GUI.h"
#include "../Components/Camera3D.h"
#include "../../imgui/imgui.h"

class GUI_Camera : public GUI  {
public:
    bool show = false;

    virtual ~GUI_Camera() {}

    virtual void draw(Camera3D *camera)
    {
        ImGuiWindowFlags window_flags = 0;

        if (show) {

            ImGui::SetNextWindowPos(ImVec2(2, 22), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 190), ImGuiSetCond_Once);
            //window_flags |= ImGuiWindowFlags_NoMove;

            std::string title = "Camera Inspector";
            ImGui::Begin(title.c_str(), &show, window_flags);

            const float range_min = EngineSetup::getInstance()->GUI_BAR_DEFAULT_MIN_VALUE;
            const float range_max = EngineSetup::getInstance()->GUI_BAR_DEFAULT_MAX_VALUE;

            const float range_sensibility = -0.01;

            const int range_framerate_min = EngineSetup::getInstance()->GUI_MIN_SPRITE3D_FRAMERATE;
            const int range_framerate_max = EngineSetup::getInstance()->GUI_MAX_SPRITE3D_FRAMERATE;

            const float range_min_yaw = 0;
            const float range_max_yaw = 0;

            const float range_min_movement = 0;
            const float range_max_movement = 500;

            const float range_min_mouse_sensitivity = 0;
            const float range_max_mouse_sensitivity = 3;

            const float test_min = 0;
            const float test_max = 100000;

            std::string position_text = "Position##1";
            std::string rotation_text = "Orientation##2";
            std::string movement_text = "Keyboard##3";
            std::string mouse_text    = "Mouse##4";
            std::string forces_text   = "Forces##5";

            // position
            if (ImGui::TreeNode( position_text.c_str() )) {
                ImGui::DragScalar("X", ImGuiDataType_Float,  &camera->getPosition()->x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("Y", ImGuiDataType_Float,  &camera->getPosition()->y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("Z", ImGuiDataType_Float,  &camera->getPosition()->z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                ImGui::TreePop();
            }

            ImGui::Separator();

            // rotation
            if (ImGui::TreeNode( rotation_text.c_str() )) {
                ImGui::DragScalar("Yaw", ImGuiDataType_Float,  &camera->yaw, range_sensibility,  &range_min_yaw, &range_max_yaw, "%f", 1.0f);
                ImGui::DragScalar("Pitch", ImGuiDataType_Float,  &camera->pitch, range_sensibility,  &range_min_yaw, &range_max_yaw, "%f", 1.0f);
                ImGui::DragScalar("Roll", ImGuiDataType_Float,  &camera->roll, range_sensibility,  &range_min_yaw, &range_max_yaw, "%f", 1.0f);
                ImGui::TreePop();
            }

            ImGui::Separator();

            if (ImGui::TreeNode( movement_text.c_str() )) {
                ImGui::DragScalar("Walking", ImGuiDataType_Float, &EngineSetup::getInstance()->WALKING_SPEED, range_sensibility,  &range_min_movement, &range_max_movement, "%f", 1.0f);
                ImGui::DragScalar("Turn", ImGuiDataType_Float,    &EngineSetup::getInstance()->TURN_SPEED, range_sensibility,  &range_min_movement, &range_max_movement, "%f", 1.0f);
                ImGui::DragScalar("Pitch", ImGuiDataType_Float,   &EngineSetup::getInstance()->PITCH_SPEED, range_sensibility,  &range_min_movement, &range_max_movement, "%f", 1.0f);
                ImGui::DragScalar("Strafe", ImGuiDataType_Float,  &EngineSetup::getInstance()->STRAFE_SPEED, range_sensibility,  &range_min_movement, &range_max_movement, "%f", 1.0f);

                ImGui::TreePop();
            }

            ImGui::Separator();

            if (ImGui::TreeNode( mouse_text.c_str() )) {
                ImGui::DragScalar("Sens.", ImGuiDataType_Float,  &EngineSetup::getInstance()->MOUSE_SENSITIVITY, range_sensibility,  &range_min_mouse_sensitivity, &range_max_mouse_sensitivity, "%f", 1.0f);
                ImGui::TreePop();
            }

            ImGui::Separator();
            ImGui::End();
        }
    }
};


#endif //BRAKEDA3D_GUI_CAMERA_H