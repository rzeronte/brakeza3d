
#ifndef SDL2_3D_ENGINE_GUI_WEAPONS_H
#define SDL2_3D_ENGINE_GUI_WEAPONS_H


#include "../Objects/LightPoint3D.h"
#include "../Misc/Tools.h"
#include "GUI.h"
#include "../../imgui/imgui.h"

class GUI_Weapons : public GUI {
public:
    bool show = false;
    int misc_flags = ImGuiColorEditFlags_NoOptions;

    virtual ~GUI_Weapons() {}

    virtual void draw() {
        /*ImGuiWindowFlags window_flags = 0;

        if (show) {
            const int range_framerate_min = EngineSetup::getInstance()->GUI_MIN_SPRITE3D_FRAMERATE;
            const int range_framerate_max = EngineSetup::getInstance()->GUI_MAX_SPRITE3D_FRAMERATE;

            std::string title = "Weapons Inspector (" + std::to_string(weapon->numWeapons) + " types)";

            ImGui::SetNextWindowPos(ImVec2(2, 437), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiSetCond_Once);
            //window_flags |= ImGuiWindowFlags_NoMove;

            ImGui::Begin(title.c_str(), &show, window_flags);

            const float  range_min = 0;
            const float  range_max = 10;
            const float  range_sensibility = 1;

            const char *items[] = {"melee", "gun", "machinegun", "rocketlauncher"};
            static const char *item_current = items[weapon->currentWeaponIndex]; // Here our selection is a single pointer stored outside the object.
            static ImGuiComboFlags flags = 0;

            if (ImGui::BeginCombo("weapon", item_current, flags)) { // The second parameter is the label previewed before opening the combo.
                for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                    bool is_selected = (item_current == items[n]);
                    if (ImGui::Selectable(items[n], is_selected)) {
                        item_current = items[n];
                    }

                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                    }

                    if (ImGui::IsItemDeactivatedAfterEdit()) {
                        weapon->currentWeaponIndex = n;
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::DragScalar("Framerate", ImGuiDataType_S32, &weapon->weaponTypes[weapon->currentWeaponIndex]->getCurrentWeaponAnimation()->fps, 1.f, &range_framerate_min, &range_framerate_max, "%d fps", 1);
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (std::string("status: ") + std::to_string((int) weapon->weaponTypes[weapon->currentWeaponIndex]->status)).c_str());
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (std::string("isFiring: ") + std::to_string((int) weapon->weaponTypes[weapon->currentWeaponIndex]->isFiring())).c_str());
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (std::string("Current Frame: ") + std::to_string(weapon->weaponTypes[weapon->currentWeaponIndex]->getCurrentWeaponAnimation()->currentFrame)).c_str());
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (std::string("NumFrames: ") + std::to_string(weapon->weaponTypes[weapon->currentWeaponIndex]->getCurrentWeaponAnimation()->getNumFrames())).c_str());
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (std::string("weapon global offsetY: ") + std::to_string(weapon->offsetY)).c_str());
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), (std::string("weapon currentAnimationIndex: ") + std::to_string(weapon->getCurrentWeaponType()->currentAnimationIndex)).c_str());

            ImGui::End();
        }*/
    }
};

#endif
