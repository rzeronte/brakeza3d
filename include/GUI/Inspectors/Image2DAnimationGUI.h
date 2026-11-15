/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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