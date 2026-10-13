//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DANIMATIONGUI_H
#define BRAKEZA3D_IMAGE3DANIMATIONGUI_H

#include "../../3D/Image3DAnimation.h"
#include "../Widgets/GUIFields.h"
#include "../AddOns/GUIAddonFilePicker.h"
#include "../../Config.h"

class Image3DAnimationGUI
{
    static inline GUIFilePicker picker;

public:
    static void DrawPropertiesGUI(Image3DAnimation *o)
    {
        if (ImGui::CollapsingHeader("Billboard Animation")) {
            using namespace GUIFields;
            if (BeginSection("SIZE")) {
                if (Float("Width", &o->width, 0.1f, 0.0f, 1000.0f, "%.2f")) {
                    o->UpdateBillboardSize();
                }
                if (Float("Height", &o->height, 0.1f, 0.0f, 1000.0f, "%.2f")) {
                    o->UpdateBillboardSize();
                }
                EndSection();
            }

            ImGui::Separator();
            if (ImGui::TreeNode("Add animation")) {
                static char name[256];
                strncpy(name, o->currentSpriteFileVariableToCreateAnimation.c_str(), sizeof(name));

                if (ImGui::InputText("Sprite Folder##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
                    o->currentSpriteFileVariableToCreateAnimation = name;
                }
                ImGui::SameLine();
                if (picker.drawTrigger("##browse_anim"))
                    picker.open(o->currentSpriteFileVariableToCreateAnimation, Config::get()->IMAGES_FOLDER, "../assets/images/", {".png", ".jpg", ".jpeg"});
                if (picker.draw("Image3DAnim_FilePicker"))
                    o->currentSpriteFileVariableToCreateAnimation = picker.result;
                ImGui::InputInt("Width", &o->currentWidthVariableToCreateAnimation);
                ImGui::InputInt("Height", &o->currentHeightVariableToCreateAnimation);
                ImGui::InputInt("Nº Frames", &o->currentFramesVariableToCreateAnimation);

                if (ImGui::Button(std::string("Load animation").c_str())) {
                    if (!o->currentSpriteFileVariableToCreateAnimation.empty()) {
                        o->CreateAnimation(
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
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DANIMATIONGUI_H