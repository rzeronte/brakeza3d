//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE2D_ANIMATIONGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../2D/Image2DAnimation.h"

class Image2DAnimationGUI
{
public:
    static void drawImGuiProperties(Image2DAnimation *o)
    {
        if (ImGui::CollapsingHeader("Image2DAnimation")) {

            const int range_min_int = 1;
            const int range_max_int = 1280;

            if (ImGui::TreeNode("Screen Position")) {
                ImGui::DragScalar("Offset X", ImGuiDataType_S32, &o->x,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
                ImGui::DragScalar("Offset Y", ImGuiDataType_S32, &o->y,1.f, &range_min_int, &range_max_int, "%d", 1.0f);

                ImGui::TreePop();
            }
            o->animation->drawImGuiProperties();
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H