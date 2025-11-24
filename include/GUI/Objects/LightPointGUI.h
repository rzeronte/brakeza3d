//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../Objects/LightPoint.h"

class LightPointGUI
{
public:
    static void drawImGuiProperties(LightPoint *o)
    {
        if (ImGui::CollapsingHeader("LightPoint")) {

            const float range_potence_sensibility = 0.01f;
            const float range_min = -90000;
            const float range_max = 90000;

            ImVec4 color = {o->diffuse.x, o->diffuse.y, o->diffuse.z, 1};
            bool changed_color = ImGui::ColorEdit4("Diffuse##", (float *) &color, ImGuiColorEditFlags_NoOptions);
            if (changed_color) {
                o->setColor(Color(color.x,color.y,color.z));
            }
            color = {o->specular.x, o->specular.y, o->specular.z, 1};
            changed_color = ImGui::ColorEdit4("Specular##", (float *) &color, ImGuiColorEditFlags_NoOptions);
            if (changed_color) {
                o->setColorSpecular(Color(color.x, color.y, color.z));
            }

            color = {o->ambient.x, o->ambient.y, o->ambient.z, 1};
            changed_color = ImGui::ColorEdit4("Ambient##", (float *) &color, ImGuiColorEditFlags_NoOptions);
            if (changed_color) {
                o->setAmbient(Color(color.x, color.y,color.z));
            }

            ImGui::DragScalar("Constant", ImGuiDataType_Float, &o->constant, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Linear", ImGuiDataType_Float, &o->linear, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Quadratic", ImGuiDataType_Float, &o->quadratic, range_potence_sensibility,&range_min, &range_max, "%f", 1.0f);
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H