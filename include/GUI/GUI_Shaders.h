//
// Created by eduardo on 3/3/22.
//

#ifndef BRAKEDA3D_GUI_SHADERS_H
#define BRAKEDA3D_GUI_SHADERS_H

#include "GUI.h"
#include "../Components/Camera3D.h"
#include "../../imgui/imgui.h"
#include "GUIManager.h"
#include "../Render/Tile.h"
#include "../Render/Shader.h"
#include "../Shaders/ShaderWater.h"
#include "../Shaders/ShaderSmoke.h"
#include "../Shaders/ShaderBlink.h"
#include "../Shaders/ShaderTintScreen.h"
#include "../Shaders/ShaderObjectSilhouette.h"
#include "../Shaders/ShaderImageBackground.h"

class GUI_Shaders : public GUI {
private:
    ImVec4 imGuiColorBlink;
    ImVec4 imGuiTintColor;
public:
    bool show = false;

    virtual ~GUI_Shaders() {}

    virtual void draw(std::map<int, Shader*> &shaders) {
        ImGuiWindowFlags window_flags = 0;

        if (show) {

            ImGui::SetNextWindowPos(ImVec2(10, 650), ImGuiSetCond_Once);
            ImGui::SetNextWindowSize(ImVec2(450, 240), ImGuiSetCond_Once);

            std::string title = "Shaders";
            ImGui::Begin(title.c_str(), &show, window_flags);

            int i = 1;
            for (auto shaderMap : shaders) {
                std::string shader_text = "Shader " + shaderMap.second->getLabel() + "##" + std::to_string(i);
                std::string enabled_text = "Enabled##" + std::to_string(i);
                auto shader = shaderMap.second;
                if (ImGui::CollapsingHeader(shader_text.c_str(), false)) {
                    ImGui::Checkbox(enabled_text.c_str(), &shader->enabled);
                    comboPhaseType(shader, i);
                    ImGui::Separator();
                    GuiShaderBackgroundImage(shader, i);
                    ImGui::Separator();
                    GuiShaderWater(shader, i);
                    ImGui::Separator();
                    GuiShaderSmoke(shader, i);
                    ImGui::Separator();
                    GuiShaderBlink(shader, i);
                    ImGui::Separator();
                    GuiShaderSilhouette(shader, i);
                    ImGui::Separator();
                    GuiShaderTint(shader, i);
                }

                i++;
            }
            ImGui::Columns(1);
            ImGui::End();
        }
    }

    void comboPhaseType(Shader *shader, int i) const {
        std::string comboLabel = "PhaseType##" + std::to_string(i);

        const char *items[] = {"PreUpdate", "PostUpdate"};
        static const char *item_current; // Here our selection is a single pointer stored outside the object.

        switch (shader->getPhaseRender()) {
            case EngineSetup::PREUPDATE:
                item_current = items[0];
                break;
            case EngineSetup::POSTUPDATE:
                item_current = items[1];
                break;
        }
        static ImGuiComboFlags flags = 0;

        // The second parameter is the label previewed before opening the combo.
        if (ImGui::BeginCombo(comboLabel.c_str(), item_current, flags)) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                bool is_selected = (item_current == items[n]);
                if (ImGui::Selectable(items[n], is_selected)) {
                    item_current = items[n];
                }

                if (is_selected) {
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                }

                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    shader->setPhaseRender(n);
                    Logging::Log(("Changing shader phaseType to  " + std::to_string(n)), "GUI");
                }
            }
            ImGui::EndCombo();
        }
    }

    void GuiShaderWater(Shader *shader, int i) {
        const float range_min = -10;
        const float range_max = 10;
        const float range_sensibility = 0.1;

        std::string rgb_itensity = "RGB Intensity##" + std::to_string(i);

        auto shaderWater = dynamic_cast<ShaderWater*> (shader);
        if (shaderWater != nullptr) {
            ImGui::DragScalar("Closeness", ImGuiDataType_Float, &(shaderWater->LAVA_CLOSENESS), range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Intensity", ImGuiDataType_Float, &(shaderWater->LAVA_INTENSITY), range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Speed", ImGuiDataType_Float, &(shaderWater->LAVA_SPEED), range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Scale", ImGuiDataType_Float, &(shaderWater->LAVA_SCALE), range_sensibility,&range_min, &range_max, "%f", 1.0f);
            if (ImGui::TreeNode(rgb_itensity.c_str())) {
                ImGui::DragScalar("R", ImGuiDataType_Float, &shaderWater->intensity_r, range_sensibility,&range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("G", ImGuiDataType_Float, &shaderWater->intensity_g, range_sensibility,&range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("B", ImGuiDataType_Float, &shaderWater->intensity_b, range_sensibility,&range_min, &range_max, "%f", 1.0f);
                ImGui::TreePop();
            }
        }
    }

    void GuiShaderSmoke(Shader *shader, int i) {
        auto shaderSmoke = dynamic_cast<ShaderSmoke*> (shader);
        if (shaderSmoke != nullptr) {
            const float range_min = 0;
            const float range_max = 5;
            const float range_sensibility = 0.01;
            comboSmokeDirection(shaderSmoke, i);
            ImGui::DragScalar("Wheel", ImGuiDataType_Float, &shaderSmoke->wheel, range_sensibility,&range_min, &range_max, "%f", 1.0f);
        }
    }

    void comboSmokeDirection(ShaderSmoke *shader, int i) const {
        std::string comboLabel = "Smoke Direction##" + std::to_string(i);

        const char *items[] = {"First", "Second", "Third", "Fourth"};
        static const char *item_current;

        switch (shader->getDirection()) {
            case ShaderSmokeDirections::FIRST:
                item_current = items[0];
                break;
            case ShaderSmokeDirections::SECOND:
                item_current = items[1];
                break;
            case ShaderSmokeDirections::THIRD:
                item_current = items[2];
                break;
            case ShaderSmokeDirections::FOURTH:
                item_current = items[3];
                break;
        }
        static ImGuiComboFlags flags = 0;

        // The second parameter is the label previewed before opening the combo.
        if (ImGui::BeginCombo(comboLabel.c_str(), item_current, flags)) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                bool is_selected = (item_current == items[n]);
                if (ImGui::Selectable(items[n], is_selected)) {
                    item_current = items[n];
                }

                if (is_selected) {
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                }

                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    shader->setDirection(n);
                    Logging::Log(("Changing shaderSmoke direction to  " + std::to_string(n)), "GUI");
                }
            }
            ImGui::EndCombo();
        }
    }


    void GuiShaderBlink(Shader *shader, int i) {
        auto shaderBlink = dynamic_cast<ShaderBlink*> (shader);
        if (shaderBlink != nullptr) {
            const float range_min = 0;
            const float range_max = 5;
            const float range_sensibility = 0.001;
            ImGui::DragScalar("Step", ImGuiDataType_Float, &shaderBlink->step, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                shaderBlink->setStep(shaderBlink->step);
            }

            std::string colorText = "Blink Color##" + std::to_string(i);
            int misc_flags = ImGuiColorEditFlags_NoOptions;
            bool hasColorChanged = ImGui::ColorEdit4(colorText.c_str(), (float *) &imGuiColorBlink, misc_flags);
            if (hasColorChanged) {
                shaderBlink->setColor(Color(
                        imGuiColorBlink.x * 256,
                        imGuiColorBlink.y * 256,
                        imGuiColorBlink.z * 256
                ));
            }
        }
    }

    void GuiShaderTint(Shader *shader, int i) {
        auto shaderTintScreen = dynamic_cast<ShaderTintScreen*> (shader);
        if (shaderTintScreen != nullptr) {
            std::string colorText = "Tint Color##" + std::to_string(i);
            int misc_flags = ImGuiColorEditFlags_NoOptions;
            bool hasColorChanged = ImGui::ColorEdit4(colorText.c_str(), (float *) &imGuiTintColor, misc_flags);
            if (hasColorChanged) {
                shaderTintScreen->setTintColorIntensity(
                imGuiTintColor.x,
                imGuiTintColor.y,
                imGuiTintColor.z
                );
            }
        }
    }

    void GuiShaderSilhouette(Shader *shader, int i) {
        auto shaderSilhouette = dynamic_cast<ShaderObjectSilhouette*> (shader);
        if (shaderSilhouette != nullptr) {
            std::string colorText = "Silhouette Color##" + std::to_string(i);
            int misc_flags = ImGuiColorEditFlags_NoOptions;
            bool hasColorChanged = ImGui::ColorEdit4(colorText.c_str(), (float *) &imGuiTintColor, misc_flags);
            if (hasColorChanged) {
                shaderSilhouette->setColor(Color(
                    imGuiTintColor.x * 256,
                    imGuiTintColor.y * 256,
                    imGuiTintColor.z * 256
                ));
            }
        }
    }

    void GuiShaderBackgroundImage(Shader *shader, int i) {
        auto shaderBackground = dynamic_cast<ShaderImageBackground*> (shader);
        if (shaderBackground != nullptr) {
            std::string autoScroll_text = "AutoScroll Speed##" + std::to_string(i);

            comboBackgroundType(shaderBackground, i);

            if (shaderBackground->getType() == ShaderImageBackgroundTypes::PORTION) {
                ImGui::Checkbox("AutoScroll Enabled", &shaderBackground->autoScrollEnabled);

                if (shaderBackground->autoScrollEnabled) {
                    if (ImGui::TreeNode(autoScroll_text.c_str())) {
                        const float range_min = -5;
                        const float range_max = 5;
                        const float range_sensibility = 0.1;
                        ImGui::DragScalar("X", ImGuiDataType_Float, &shaderBackground->getAutoScrollSpeed().x, range_sensibility,&range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_Float, &shaderBackground->getAutoScrollSpeed().y, range_sensibility,&range_min, &range_max, "%f", 1.0f);
                        ImGui::TreePop();
                    }
                }

                std::string portionSetup_text = "Portion Setup##" + std::to_string(i);
                if (ImGui::TreeNode(portionSetup_text.c_str())) {
                    const float range_min = 0;
                    const float range_max = 1024;
                    const float range_sensibility = 1;

                    ImGui::DragScalar("X Draw Position", ImGuiDataType_S32, &shaderBackground->xDrawPos, range_sensibility,&range_min, &range_max, "%f", 1.0f);
                    ImGui::DragScalar("Y Draw Position", ImGuiDataType_S32, &shaderBackground->yDrawPos, range_sensibility,&range_min, &range_max, "%f", 1.0f);

                    ImGui::DragScalar("X Offset Image", ImGuiDataType_S32, &shaderBackground->xImage, range_sensibility,&range_min, &range_max, "%f", 1.0f);
                    ImGui::DragScalar("Y Offset Image", ImGuiDataType_S32, &shaderBackground->yImage, range_sensibility,&range_min, &range_max, "%f", 1.0f);

                    ImGui::DragScalar("Portion Width", ImGuiDataType_S32, &shaderBackground->wImage, range_sensibility,&range_min, &range_max, "%f", 1.0f);
                    ImGui::DragScalar("Portion Height", ImGuiDataType_S32, &shaderBackground->hImage, range_sensibility,&range_min, &range_max, "%f", 1.0f);
                    ImGui::TreePop();
                }
            }
        }
    }

    void comboBackgroundType(ShaderImageBackground *shader, int i) const {
        std::string comboLabel = "Background Type##" + std::to_string(i);

        const char *items[] = {"Fullscreen", "Portion", "Center"};
        static const char *item_current;

        switch (shader->getType()) {
            case ShaderImageBackgroundTypes::FULLSCREEN:
                item_current = items[0];
                break;
            case ShaderImageBackgroundTypes::PORTION:
                item_current = items[1];
                break;
            case ShaderImageBackgroundTypes::CENTER:
                item_current = items[2];
                break;
        }
        static ImGuiComboFlags flags = 0;

        // The second parameter is the label previewed before opening the combo.
        if (ImGui::BeginCombo(comboLabel.c_str(), item_current, flags)) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                bool is_selected = (item_current == items[n]);
                if (ImGui::Selectable(items[n], is_selected)) {
                    item_current = items[n];
                }

                if (is_selected) {
                    ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                }

                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    shader->setType(n);
                    Logging::Log(("Changing ShaderBackgroundImage type to  " + std::to_string(n)), "GUI");
                }
            }
            ImGui::EndCombo();
        }
    }
};


#endif //BRAKEDA3D_GUI_SHADERS_H
