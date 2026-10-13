//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DANIMATION8DIRGUI_H
#define BRAKEZA3D_IMAGE3DANIMATION8DIRGUI_H

#include "../../3D/Image3DAnimation360.h"
#include "../Widgets/GUIFields.h"
#include "../AddOns/GUIAddonFilePicker.h"
#include "../../Config.h"

class Image3DAnimation360GUI
{
    static inline GUIFilePicker picker;

public:
    static void DrawPropertiesGUI(Image3DAnimation360 *o)
    {
        if (ImGui::CollapsingHeader("BillboardAnimation8Directions")) {
            using namespace GUIFields;
            if (BeginSection("SIZE")) {
                if (Float("Width", &o->width, 0.1f, 0.0f, 1000.0f, "%.2f")) {
                }
                if (Float("Height", &o->height, 0.1f, 0.0f, 1000.0f, "%.2f")) {
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
                if (picker.drawTrigger("##browse_anim360"))
                    picker.open(o->currentSpriteFileVariableToCreateAnimation, Config::get()->IMAGES_FOLDER, "../assets/images/", {".png", ".jpg", ".jpeg"});
                if (picker.draw("Image3DAnim360_FilePicker"))
                    o->currentSpriteFileVariableToCreateAnimation = picker.result;
                if (ImGui::InputInt("Num. Frames", &o->currentFramesVariableToCreateAnimation)) {
                }

                if (ImGui::Button(std::string("Load directional animation").c_str())) {
                    o->CreateAnimationDirectional2D(
                        new TextureAnimatedDirectional(
                        o->currentSpriteFileVariableToCreateAnimation,
                        o->currentFramesVariableToCreateAnimation,
                        24,
                        -1
                        ),
                        false
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
                ImGui::Combo("Animation", &o->currentIndexAnimation, items, IM_ARRAYSIZE(items));
            } else {
                ImGui::Text("No animations found!");
            }
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DANIMATION8DIRGUI_H