//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_LIGHTSPOTDGUI_H
#define BRAKEZA3D_LIGHTSPOTDGUI_H

#include "../../3D/LightSpot.h"
#include "../../GUI/Objects/Object3DGUI.h"

class LightSpotGUI
{
public:
    static void DrawPropertiesGUI(LightSpot *o)
    {
        if (ImGui::CollapsingHeader("SpotLight")) {
            const float range_sensibility = 0.001f;
            const float range_min = 0;
            const float range_max = 1;

            ImGui::DragScalar("Cut Off", ImGuiDataType_Float, &o->cutOff, range_sensibility, &range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Outer Cut Off", ImGuiDataType_Float, &o->outerCutOff, range_sensibility, &range_min, &range_max, "%f", 1.0f);
            ImGui::Separator();
            ImGui::Checkbox("Show Debug Cone", &o->showDebugCone);

            if (o->showDebugCone) {
                ImGui::Text("Blend Debug Mode Settings");

                Object3DGUI::SelectorOGLBlendMode(o->getRenderSettings().mode_src, o->getRenderSettings().mode_dst);
            }
        }
    }
};

#endif //BRAKEZA3D_LIGHTSPOTDGUI_H