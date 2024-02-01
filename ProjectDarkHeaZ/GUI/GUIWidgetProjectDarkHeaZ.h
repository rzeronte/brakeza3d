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
            auto hud = ComponentsManager::get()->getComponentHUD();

            ImGui::Checkbox("GOD Mode", &ComponentsManager::get()->getComponentGame()->getPlayer()->god);
            ImGui::Separator();

            if (ImGui::TreeNode("HUD")) {
                const float range_min = 1280;
                const float range_max = 0;

                ImGui::DragScalar("Global Offset X", ImGuiDataType_S32, &hud->globalOffset.x, 1.0f ,&range_min, &range_max, "%d", 1.0f);
                ImGui::DragScalar("Global Offset Y", ImGuiDataType_S32, &hud->globalOffset.y, 1.0f ,&range_min, &range_max, "%d", 1.0f);

                if (ImGui::TreeNode("Resources Offset")) {
                    ImGui::DragScalar("X", ImGuiDataType_S32, &hud->resourcesOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->resourcesOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Health Icon Offset")) {
                    ImGui::DragScalar("X", ImGuiDataType_S32, &hud->iconHealthOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->iconHealthOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Energy Icon Offset")) {
                    ImGui::DragScalar("X", ImGuiDataType_S32, &hud->iconEnergyOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->iconEnergyOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Weapons Icons")) {
                    if (ImGui::TreeNode("Global Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->offsetIcons.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->offsetIcons.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Weapon Icon Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->weaponIconOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->weaponIconOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Ammo Amount Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->ammoAmountOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->ammoAmountOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Bomb Icon Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->bombIconOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->bombIconOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Ammo Bomb Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->ammoBombOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->ammoBombOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Shield Icon Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->shieldIconOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->shieldIconOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Ammo Shield Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->ammoShieldOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->ammoShieldOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Reflection Icon Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->reflectionIconOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->reflectionIconOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Ammo Reflection Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->ammoReflectionOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->ammoReflectionOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Level Number Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->levelNumberOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->levelNumberOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Enemy info Offset")) {
                    ImGui::DragScalar("X", ImGuiDataType_S32, &hud->avatarEnemyOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->avatarEnemyOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Bars settings")) {
                    const float rangeBarMin = -500000;
                    const float rangeBarMax = 500000;
                    const float rangeBarSens = 0.1;

                    ImGui::DragScalar("Stroke HUD Bars Weight", ImGuiDataType_Float, &hud->strokeBars, rangeBarSens ,&rangeBarMin, &rangeBarMax, "%f", 1.0f);
                    ImGui::DragScalar("Stroke HUD Bars Intensity", ImGuiDataType_Float, &hud->intensityBars, rangeBarSens ,&rangeBarMin, &rangeBarMax, "%f", 1.0f);

                    if (ImGui::TreeNode("Stamina Bar Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->staminaOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->staminaOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Energy Bar Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->energyOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->energyOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Enemy Stamina Bar Offset")) {
                        ImGui::DragScalar("X", ImGuiDataType_S32, &hud->staminaEnemyOffset.x, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_S32, &hud->staminaEnemyOffset.y, 1.0f, &range_min, &range_max, "%d", 1.0f);
                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Game Levels")) {
                auto levels = ComponentsManager::get()->getComponentGame()->getLevelLoader()->getLevels();
                for (int n = 0; n < levels.size(); n++) {
                    auto levelName = std::to_string(n + 1) + ") " + levels[n];
                    ImGui::Button(levelName.c_str());
                    if (ImGui::IsItemClicked()) {
                        ComponentsManager::get()->getComponentGame()->reloadLevel(n);
                    }
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
};
#endif //BRAKEZA3D_GUIWIDGETPROJECTDARKHEAZ_H
