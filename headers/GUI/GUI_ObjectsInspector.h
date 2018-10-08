
#ifndef SDL2_3D_ENGINE_GUI_INSPECTOR_H
#define SDL2_3D_ENGINE_GUI_INSPECTOR_H


#include "GUI.h"
#include "../../imgui/imgui.h"
#include "../../headers/Object3D.h"
#include "../../headers/Mesh.h"

class GUI_ObjectsInspector : public GUI  {
public:
    bool show = true;

    virtual ~GUI_ObjectsInspector() {}

    virtual void draw(Object3D **objects, int number)
    {
        ImGuiWindowFlags window_flags = 0;

        if (show) {
            std::string title = "Object Inspector (" + std::to_string(number) + " objects)";

            ImGui::SetNextWindowPos(ImVec2(2, 22), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 420), ImGuiSetCond_Once);
            window_flags |= ImGuiWindowFlags_NoMove;
            bool *open;
            ImGui::Begin(title.c_str(), open, window_flags);

            const float  range_min = EngineSetup::getInstance()->GUI_MIN_SPACE_COORDINATES_VALUE;
            const float  range_max = EngineSetup::getInstance()->GUI_MAX_SPACE_COORDINATES_VALUE;
            const float  range_sensibility = EngineSetup::getInstance()->GUI_FLOAT_SPACE_COORDINATES_SENS;

            for (int i = 0; i < number; i++) {
                std::string header_text = objects[i]->label + "##" + std::to_string(i);
                std::string enabled_text = "Enabled##" + std::to_string(i);
                std::string position_text = "Position##" + std::to_string(i);
                std::string rotation_text = "Rotation##" + std::to_string(i);
                std::string shadow_text = "Shadow##" + std::to_string(i);

                if (ImGui::CollapsingHeader(header_text.c_str(), i)) {
                    if (ImGui::TreeNode( position_text.c_str() )) {
                        ImGui::DragScalar("X",     ImGuiDataType_Float,  &objects[i]->position.x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Y",     ImGuiDataType_Float,  &objects[i]->position.y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Z",     ImGuiDataType_Float,  &objects[i]->position.z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode( rotation_text.c_str() )) {
                        ImGui::DragScalar("X",     ImGuiDataType_Float,  &objects[i]->rotation.x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Y",     ImGuiDataType_Float,  &objects[i]->rotation.y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Z",     ImGuiDataType_Float,  &objects[i]->rotation.z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                        ImGui::TreePop();
                    }

                    ImGui::Checkbox(shadow_text.c_str(), &dynamic_cast<Mesh *>(objects[i])->shadowCaster);
                    ImGui::Checkbox(enabled_text.c_str(), &dynamic_cast<Mesh *>(objects[i])->enabled);
                }
            }

            ImGui::End();
        }
    }
};


#endif //SDL2_3D_ENGINE_GUI_INSPECTOR_H
