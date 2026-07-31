//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_LIGHTSPOTDGUI_H
#define BRAKEZA3D_LIGHTSPOTDGUI_H

#include <cmath>
#include <algorithm>
#include "../../3D/LightSpot.h"
#include "../../GUI/Objects/Object3DGUI.h"

class LightSpotGUI
{
public:
    static void DrawPropertiesGUI(LightSpot *o)
    {
        if (ImGui::CollapsingHeader("SpotLight")) {
            ImGui::PushID(o);

            // Cone stored as cosine of the HALF angle; edit as full cone angle in degrees.
            const float RAD2DEG = 57.2957795f;
            const float DEG2RAD = 0.01745329252f;

            float innerCos = std::clamp(o->cutOff,      -1.0f, 1.0f);
            float outerCos = std::clamp(o->outerCutOff, -1.0f, 1.0f);
            float innerDeg = std::acos(innerCos) * 2.0f * RAD2DEG;
            float outerDeg = std::acos(outerCos) * 2.0f * RAD2DEG;

            bool innerChanged = ImGui::SliderFloat("Inner cone (deg)", &innerDeg, 0.0f, 179.0f, "%.1f");
            bool outerChanged = ImGui::SliderFloat("Outer cone (deg)", &outerDeg, 0.0f, 179.0f, "%.1f");

            // Enforce inner <= outer to keep epsilon (cutOff - outerCutOff) positive
            if (innerChanged && innerDeg > outerDeg) outerDeg = innerDeg;
            if (outerChanged && outerDeg < innerDeg) innerDeg = outerDeg;

            if (innerChanged || outerChanged) {
                o->cutOff      = std::cos(innerDeg * 0.5f * DEG2RAD);
                o->outerCutOff = std::cos(outerDeg * 0.5f * DEG2RAD);
            }

            ImGui::Separator();
            ImGui::Checkbox("Show Debug Cone", &o->showDebugCone);
            bool cs = o->getCastsShadow();
            if (ImGui::Checkbox("Casts Shadow", &cs)) o->setCastsShadow(cs);

            ImGui::PopID();
        }
    }
};

#endif //BRAKEZA3D_LIGHTSPOTDGUI_H
