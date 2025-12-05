//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DANIMATION8DIRGUI_H
#define BRAKEZA3D_IMAGE3DANIMATION8DIRGUI_H

#include "../../3D/Image3DAnimation360.h"

class Image3DAnimation360GUI
{
public:
    static void DrawPropertiesGUI(Image3DAnimation360 *o)
    {
        if (ImGui::CollapsingHeader("BillboardAnimation8Directions")) {
            if (ImGui::TreeNode("Size")) {
                const float range_min = 0;
                const float range_max = 1000;
                const float range_sensibility = 0.1;

                if (ImGui::DragScalar("Width", ImGuiDataType_Float, &o->width, range_sensibility ,&range_min, &range_max, "%f", 1.0f)){
                }
                if (ImGui::DragScalar("Height", ImGuiDataType_Float, &o->height, range_sensibility ,&range_min, &range_max, "%f", 1.0f)) {
                }
                ImGui::TreePop();
            }

            ImGui::Separator();
            if (ImGui::TreeNode("Add animation")) {
                static char name[256];
                strncpy(name, o->currentSpriteFileVariableToCreateAnimation.c_str(), sizeof(name));

                if (ImGui::InputText("Sprite Folder##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
                    o->currentSpriteFileVariableToCreateAnimation = name;
                }
                if (ImGui::InputInt("Num. Frames", &o->currentFramesVariableToCreateAnimation)) {
                }

                if (ImGui::Button(std::string("Load directional animation").c_str())) {
                    o->addAnimationDirectional2D(
                        o->currentSpriteFileVariableToCreateAnimation,
                        o->currentFramesVariableToCreateAnimation,
                        24,
                        false,
                        -1
                    );
                    o->setAnimation(o->animations.size()-1);
                    o->currentSpriteFileVariableToCreateAnimation = "";
                    o->currentFramesVariableToCreateAnimation = 0;
                }

                ImGui::TreePop();
            }

            ImGui::Separator();

            const char* items[o->animations.size()];
            for (unsigned int i = 0; i < o->animations.size(); i++) {
                items[i] = o->animations[i]->getBaseFile().c_str();
            }

            if (!o->animations.empty()) {
                ImGui::Combo("Animation", &o->currentAnimation, items, IM_ARRAYSIZE(items));
            } else {
                ImGui::Text("No animations found!");
            }
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DANIMATION8DIRGUI_H