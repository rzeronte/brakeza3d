
#ifndef BRAKEDA3D_GUI_CAMERA_H
#define BRAKEDA3D_GUI_CAMERA_H


#include "GUI.h"
#include "../../headers/Objects/Camera3D.h"
#include "../../imgui/imgui.h"

class GUI_Camera : public GUI  {
public:
    bool show = true;

    virtual ~GUI_Camera() {}

    virtual void draw(Camera3D *camera)
    {
        ImGuiWindowFlags window_flags = 0;

        if (show) {

            ImGui::SetNextWindowPos(ImVec2(260, 22), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 160), ImGuiSetCond_Once);
            window_flags |= ImGuiWindowFlags_NoMove;

            std::string title = "Camera Inspector";
            ImGui::Begin(title.c_str(), &show, window_flags);

            const float range_min = EngineSetup::getInstance()->GUI_MIN_SPACE_COORDINATES_VALUE;
            const float range_max = EngineSetup::getInstance()->GUI_MAX_SPACE_COORDINATES_VALUE;
            const float range_sensibility = EngineSetup::getInstance()->GUI_FLOAT_SPACE_COORDINATES_SENS;
            const float range_sensibility_test = 1;

            const int range_framerate_min = EngineSetup::getInstance()->GUI_MIN_SPRITE3D_FRAMERATE;
            const int range_framerate_max = EngineSetup::getInstance()->GUI_MAX_SPRITE3D_FRAMERATE;

            const float range_min_yaw = 0;
            const float range_max_yaw = 0;

            const float test_min = 0;
            const float test_max = 100000;

            std::string position_text = "Position##1";
            std::string rotation_text = "Orientation##2";
            std::string test_text = "Test value##3";

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
                ImGui::TreePop();
            }

            ImGui::Checkbox("Mouse Rotation", &EngineSetup::getInstance()->CAMERA_MOUSE_ROTATION);

            ImGui::End();
        }
    }
};


#endif //BRAKEDA3D_GUI_CAMERA_H
