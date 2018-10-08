
#ifndef SDL2_3D_ENGINE_GUI_LIGHTSINSPECTOR_H
#define SDL2_3D_ENGINE_GUI_LIGHTSINSPECTOR_H


#include "../../headers/LightPoint.h"
#include "../../headers/Tools.h"
#include "GUI.h"
#include "../../imgui/imgui.h"

class GUI_LightsInspector : public GUI  {
        public:
        bool show = true;
        int misc_flags = ImGuiColorEditFlags_NoOptions;

        virtual ~GUI_LightsInspector() {}

        virtual void draw(LightPoint **lightpoints, int number)
        {
            ImGuiWindowFlags window_flags = 0;

            if (show) {

                std::string title = "Lights Inspector (" + std::to_string(number) + " lights)";

                ImGui::SetNextWindowPos(ImVec2(2, 450), ImGuiSetCond_Once);
                ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiSetCond_Once);
                window_flags |= ImGuiWindowFlags_NoMove;
                bool *open;

                ImGui::Begin(title.c_str(), open, window_flags);

                const float  range_min = EngineSetup::getInstance()->GUI_MIN_SPACE_COORDINATES_VALUE;
                const float  range_max = EngineSetup::getInstance()->GUI_MAX_SPACE_COORDINATES_VALUE;
                const float  range_sensibility = EngineSetup::getInstance()->GUI_FLOAT_SPACE_COORDINATES_SENS;

                for (int i = 0; i < number; i++) {
                    bool changed_color = false;
                    std::string position_text = "Position##" + std::to_string(i);
                    std::string rotation_text = "Rotation##" + std::to_string(i);
                    std::string attenuation_text = "Attenuation##" + std::to_string(i);
                    std::string colorpicker_text = "RGB##" + std::to_string(i);
                    std::string enabled_text = "Enabled##" + std::to_string(i);

                    if (ImGui::CollapsingHeader(lightpoints[i]->label.c_str(), i)) {

                        ImGui::Checkbox(enabled_text.c_str(), &(lightpoints[i])->enabled);

                        if (ImGui::TreeNode( position_text.c_str() )) {
                            ImGui::DragScalar("X",     ImGuiDataType_Float,  &lightpoints[i]->position.x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Y",     ImGuiDataType_Float,  &lightpoints[i]->position.y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Z",     ImGuiDataType_Float,  &lightpoints[i]->position.z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode( rotation_text.c_str() )) {
                            ImGui::DragScalar("X",     ImGuiDataType_Float,  &lightpoints[i]->rotation.x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Y",     ImGuiDataType_Float,  &lightpoints[i]->rotation.y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Z",     ImGuiDataType_Float,  &lightpoints[i]->rotation.z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode( attenuation_text.c_str() )) {
                            ImGui::DragScalar("Constant",   ImGuiDataType_Float,  &lightpoints[i]->kc, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Linear",     ImGuiDataType_Float,  &lightpoints[i]->kl, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Quadratic",  ImGuiDataType_Float,  &lightpoints[i]->kq, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::TreePop();
                        }

                        changed_color = ImGui::ColorEdit4(colorpicker_text.c_str(), (float*)&lightpoints[i]->imgui_color, misc_flags);

                        if (changed_color) {

                            lightpoints[i]->setColor(
                                lightpoints[i]->imgui_color.x*256,
                                lightpoints[i]->imgui_color.y*256,
                                lightpoints[i]->imgui_color.z*256
                             );
                        }
                    }
                }
                ImGui::End();
            }


        }
    };


#endif //SDL2_3D_ENGINE_GUI_LIGHTSINSPECTOR_H
