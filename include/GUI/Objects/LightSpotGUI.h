//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_LIGHTSPOTDGUI_H
#define BRAKEZA3D_LIGHTSPOTDGUI_H

#include "../../3D/LightSpot.h"

class LightSpotGUI
{
public:
    static void drawImGuiProperties(LightSpot *o)
    {
        if (ImGui::CollapsingHeader("SpotLight")) {
            if (ImGui::TreeNode("Direction##")) {
                const float range_sensibility = 0.01f;
                const float range_min = 0.0f;
                const float range_max = 1.0f;
                ImGui::DragScalar("x", ImGuiDataType_Float, &o->direction.x, range_sensibility, &range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("y", ImGuiDataType_Float, &o->direction.y, range_sensibility, &range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("z", ImGuiDataType_Float, &o->direction.z, range_sensibility, &range_min, &range_max, "%f", 1.0f);
                ImGui::TreePop();
            }

            const float range_sensibility = 0.01f;
            const float range_min = -10;
            const float range_max = 10;
            ImGui::DragScalar("cutOff", ImGuiDataType_Float, &o->cutOff, range_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("outerCutOff", ImGuiDataType_Float, &o->outerCutOff, range_sensibility,&range_min, &range_max, "%f", 1.0f);
        }
    }
};

#endif //BRAKEZA3D_LIGHTSPOTDGUI_H