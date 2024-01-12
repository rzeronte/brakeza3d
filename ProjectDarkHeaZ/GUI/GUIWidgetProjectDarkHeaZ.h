//
// Created by edu on 31/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETPROJECTDARKHEAZ_H
#define BRAKEZA3D_GUIWIDGETPROJECTDARKHEAZ_H

#include "imgui.h"
#include "../../include/ComponentsManager.h"

struct GUIWidgetProjectDarkHeaZ
{
    static void draw() {
        if (ImGui::Begin("Project DarkHeaZ")) {
            if (ImGui::TreeNode("HUD Offset")) {
                const float range_min = 1280;
                const float range_max = 0;

                ImGui::DragScalar("Offset X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->offsetX, 1.0f ,&range_min, &range_max, "%d", 1.0f);
                ImGui::DragScalar("Offset Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->offsetY, 1.0f ,&range_min, &range_max, "%d", 1.0f);

                const float rangeBarMin = -500000;
                const float rangeBarMax = 500000;
                const float rangeBarSens = 0.1;

                ImGui::DragScalar("Stroke HUD Bars Weight", ImGuiDataType_Float, &ComponentsManager::get()->getComponentHUD()->strokeBars, rangeBarSens ,&rangeBarMin, &rangeBarMax, "%f", 1.0f);
                ImGui::DragScalar("Stroke HUD Bars Intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentHUD()->intensityBars, rangeBarSens ,&rangeBarMin, &rangeBarMax, "%f", 1.0f);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Levels")) {
                auto levels = ComponentsManager::get()->getComponentGame()->getLevelLoader()->getLevels();
                for (int n = 0; n < levels.size(); n++) {
                    auto levelName = std::to_string(n + 1) + ") " + levels[n];
                    ImGui::Button(levelName.c_str());
                    if (ImGui::IsItemClicked()) {
                        ComponentsManager::get()->getComponentGame()->reloadLevel(n);
                    }
                }
            }

            ImGui::End();
        }
    }
};
#endif //BRAKEZA3D_GUIWIDGETPROJECTDARKHEAZ_H
