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
            if (ImGui::TreeNode("HUD")) {
                const float range_min = 1280;
                const float range_max = 0;

                ImGui::DragScalar("Global Offset X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->globalOffset.x, 1.0f ,&range_min, &range_max, "%d", 1.0f);
                ImGui::DragScalar("Global Offset Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->globalOffset.y, 1.0f ,&range_min, &range_max, "%d", 1.0f);

                if (ImGui::TreeNode("Resources Offset")) {
                    ImGui::DragScalar("X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->resourcesOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::DragScalar("Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->resourcesOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Health Icon Offset")) {
                    ImGui::DragScalar("X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->iconHealthOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::DragScalar("Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->iconHealthOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Energy Icon Offset")) {
                    ImGui::DragScalar("X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->iconEnergyOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::DragScalar("Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->iconEnergyOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Offset Weapons Icons")) {
                    ImGui::DragScalar("X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->offsetIcons.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::DragScalar("Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->offsetIcons.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Enemy info Offset")) {
                    ImGui::DragScalar("X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->avatarEnemyOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::DragScalar("Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->avatarEnemyOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Bars settings")) {
                    const float rangeBarMin = -500000;
                    const float rangeBarMax = 500000;
                    const float rangeBarSens = 0.1;

                    ImGui::DragScalar("Stroke HUD Bars Weight", ImGuiDataType_Float, &ComponentsManager::get()->getComponentHUD()->strokeBars, rangeBarSens ,&rangeBarMin, &rangeBarMax, "%f", 1.0f);
                    ImGui::DragScalar("Stroke HUD Bars Intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentHUD()->intensityBars, rangeBarSens ,&rangeBarMin, &rangeBarMax, "%f", 1.0f);

                    if (ImGui::TreeNode("Stamina Bar Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->staminaOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->staminaOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Energy Bar Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->energyOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->energyOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Enemy Stamina Bar Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->staminaEnemyOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &ComponentsManager::get()->getComponentHUD()->staminaEnemyOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }

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
