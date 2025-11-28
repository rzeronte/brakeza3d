//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_LIGHTSPOTDGUI_H
#define BRAKEZA3D_LIGHTSPOTDGUI_H

#include "../../3D/LightSpot.h"

class LightSpotGUI
{
public:
    static void DrawPropertiesGUI(LightSpot *o)
    {
        if (ImGui::CollapsingHeader("SpotLight")) {
            const float range_sensibility = 0.001f;
            const float range_min = 0;
            const float range_max = 1;

            ImGui::DragScalar("Cut Off", ImGuiDataType_Float, &o->cutOff, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Outer Cut Off", ImGuiDataType_Float, &o->outerCutOff, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::Separator();
            ImGui::Checkbox("Show Debug Cone", &o->showDebugCone);
        }
    }
};

#endif //BRAKEZA3D_LIGHTSPOTDGUI_H