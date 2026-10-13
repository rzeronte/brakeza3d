//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE2D_ANIMATIONGUI_H
#define BRAKEZA3D_IMAGE2D_ANIMATIONGUI_H

#include "../../2D/Image2DAnimation.h"
#include "../Widgets/GUIFields.h"

class Image2DAnimationGUI
{
public:
    static void DrawPropertiesGUI(Image2DAnimation *o)
    {
        if (ImGui::CollapsingHeader("Image2DAnimation")) {

            if (GUIFields::BeginSection("SCREEN POSITION")) {
                GUIFields::Int("Offset X", &o->x, 1.0f, 1, 1280, "%d", "Position in pixels.");
                GUIFields::Int("Offset Y", &o->y, 1.0f, 1, 1280, "%d", "Position in pixels.");
                GUIFields::EndSection();
            }
            o->animation->drawImGuiProperties();
        }
    }
};

#endif //BRAKEZA3D_IMAGE2D_ANIMATIONGUI_H