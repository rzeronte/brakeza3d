//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../2D/TextureAnimated.h"

class TextureAnimatedDrawerGUI
{
public:
    static void drawImGuiProperties(TextureAnimated *o)
    {
        if (ImGui::TreeNode("Current Animation Setup")) {
            static int width = o->currentSpriteWidth;
            static int height = o->currentspriteHeight;
            static int numFrames = o->numberFramesToLoad;
            static int fps = o->fps;

            const int range_min_int = 1;
            const int range_max_int = 1280;

            ImGui::DragScalar("Sprite Width", ImGuiDataType_S32, &width,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Sprite Height", ImGuiDataType_S32, &height,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Number frames", ImGuiDataType_S32, &numFrames,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("FPS", ImGuiDataType_S32, &fps,1.f, &range_min_int, &range_max_int, "%d", 1.0f);

            if (ImGui::Button("Apply")) {
                o->setup(o->getBaseFilename(), width, height, numFrames, fps);
                o->updateStep();
            }
            ImGui::TreePop();
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H