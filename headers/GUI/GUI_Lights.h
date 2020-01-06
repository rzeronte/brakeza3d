
#ifndef SDL2_3D_ENGINE_GUI_LIGHTSINSPECTOR_H
#define SDL2_3D_ENGINE_GUI_LIGHTSINSPECTOR_H


#include "../Objects/LightPoint3D.h"
#include "../Misc/Tools.h"
#include "GUI.h"
#include "../../imgui/imgui.h"

class GUI_Lights : public GUI  {
        public:
        bool show = false;
        int misc_flags = ImGuiColorEditFlags_NoOptions;

        virtual ~GUI_Lights() {}

        virtual void draw(std::vector<LightPoint3D *> &lightPoints)
        {
            ImGuiWindowFlags window_flags = 0;

            if (show) {

                std::string title = "Lights Inspector (" + std::to_string(lightPoints.size()) + " lights)";

                ImGui::SetNextWindowPos(ImVec2(2, 437), ImGuiSetCond_Once);
                ImGui::SetNextWindowSize(ImVec2(250, 167), ImGuiSetCond_Once);
                //window_flags |= ImGuiWindowFlags_NoMove;

                ImGui::Begin(title.c_str(), &show, window_flags);

                const float  range_min = EngineSetup::getInstance()->GUI_BAR_DEFAULT_MIN_VALUE;
                const float  range_max = EngineSetup::getInstance()->GUI_BAR_DEFAULT_MAX_VALUE;
                const float  range_sensibility = EngineSetup::getInstance()->GUI_BAR_SENSITIVITY;

                for (int i = 0; i < lightPoints.size(); i++) {
                    bool changed_color = false;
                    std::string position_text = "Position##" + std::to_string(i);
                    std::string rotation_text = "Rotation##" + std::to_string(i);
                    std::string attenuation_text = "Attenuation##" + std::to_string(i);
                    std::string colorpicker_text = "RGB##" + std::to_string(i);
                    std::string enabled_text = "Enabled##" + std::to_string(i);

                    if (ImGui::CollapsingHeader(lightPoints[i]->label.c_str(), i)) {

                        ImGui::Checkbox(enabled_text.c_str(), &(lightPoints[i])->enabled);

                        if (ImGui::TreeNode( position_text.c_str() )) {
                            ImGui::DragScalar("X",     ImGuiDataType_Float,  &lightPoints[i]->getPosition()->x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Y",     ImGuiDataType_Float,  &lightPoints[i]->getPosition()->y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Z",     ImGuiDataType_Float,  &lightPoints[i]->getPosition()->z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode( rotation_text.c_str() )) {
                            //ImGui::DragScalar("X",     ImGuiDataType_Float,  &lightpoints[i]->getRotation()->x, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            //ImGui::DragScalar("Y",     ImGuiDataType_Float,  &lightpoints[i]->getRotation()->y, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            //ImGui::DragScalar("Z",     ImGuiDataType_Float,  &lightpoints[i]->getRotation()->z, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode( attenuation_text.c_str() )) {
                            ImGui::DragScalar("Constant",   ImGuiDataType_Float,  &lightPoints[i]->kc, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Linear",     ImGuiDataType_Float,  &lightPoints[i]->kl, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::DragScalar("Quadratic",  ImGuiDataType_Float,  &lightPoints[i]->kq, range_sensibility,  &range_min, &range_max, "%f", 1.0f);
                            ImGui::TreePop();
                        }

                        changed_color = ImGui::ColorEdit4(colorpicker_text.c_str(), (float*)&lightPoints[i]->imgui_color, misc_flags);

                        if (changed_color) {

                            lightPoints[i]->setColor(
                                    lightPoints[i]->imgui_color.x*256,
                                    lightPoints[i]->imgui_color.y*256,
                                    lightPoints[i]->imgui_color.z*256
                             );
                        }
                    }
                }
                ImGui::End();
            }
        }
    };


#endif //SDL2_3D_ENGINE_GUI_LIGHTSINSPECTOR_H
