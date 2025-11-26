//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DANIMATIONGUI_H
#define BRAKEZA3D_IMAGE3DANIMATIONGUI_H

#include "../../3D/Image3DAnimation.h"

class Image3DAnimationGUI
{
public:
    static void DrawPropertiesGUI(Image3DAnimation *o)
    {
        if (ImGui::CollapsingHeader("Billboard Animation")) {
            if (ImGui::TreeNode("Size")) {
                const float range_min = 0;
                const float range_max = 1000;
                const float range_sensibility = 0.1;

                if (ImGui::DragScalar("Width", ImGuiDataType_Float, &o->width, range_sensibility ,&range_min, &range_max, "%f", 1.0f)){
                    o->updateBillboardSize();
                }
                if (ImGui::DragScalar("Height", ImGuiDataType_Float, &o->height, range_sensibility ,&range_min, &range_max, "%f", 1.0f)) {
                    o->updateBillboardSize();
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
                ImGui::InputInt("Width", &o->currentWidthVariableToCreateAnimation);
                ImGui::InputInt("Height", &o->currentHeightVariableToCreateAnimation);
                ImGui::InputInt("Nº Frames", &o->currentFramesVariableToCreateAnimation);

                if (ImGui::Button(std::string("Load animation").c_str())) {
                    if (!o->currentSpriteFileVariableToCreateAnimation.empty()) {
                        o->addAnimation(
                            o->currentSpriteFileVariableToCreateAnimation,
                            o->currentWidthVariableToCreateAnimation,
                            o->currentHeightVariableToCreateAnimation,
                            o->currentFramesVariableToCreateAnimation,
                            24
                        );
                        o->setAnimation(o->animations.size() - 1);
                        o->currentSpriteFileVariableToCreateAnimation = "";
                        o->currentFramesVariableToCreateAnimation = 0;
                        o->currentWidthVariableToCreateAnimation = 0;
                        o->currentHeightVariableToCreateAnimation = 0;
                    }
                }

                ImGui::TreePop();
            }
            ImGui::Separator();

            const char* items[(int)o->animations.size()];
            for (unsigned int i = 0; i < o->animations.size(); i++) {
                items[i] = o->animations[i]->getBaseFilename().c_str();
            }
            ImGui::Combo("Animation", &o->currentAnimationIndex, items, IM_ARRAYSIZE(items));

            o->getCurrentTextureAnimation()->drawImGuiProperties();

            ImGui::TreePop();
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DANIMATIONGUI_H