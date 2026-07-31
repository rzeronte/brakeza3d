//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_LIGHTPOINTGUI_H
#define BRAKEZA3D_LIGHTPOINTGUI_H

#include <cmath>
#include <algorithm>
#include "../../3D/LightPoint.h"

class LightPointGUI
{
public:
    static void DrawPropertiesGUI(LightPoint *o)
    {
        if (ImGui::CollapsingHeader("LightPoint")) {
            ImGui::PushID(o);

            ImVec4 color = {o->diffuse.x, o->diffuse.y, o->diffuse.z, 1};
            if (ImGui::ColorEdit4("Diffuse##", (float *) &color, ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoInputs)) {
                o->setColor(Color(color.x, color.y, color.z));
            }
            ImGui::SameLine();
            color = {o->specular.x, o->specular.y, o->specular.z, 1};
            if (ImGui::ColorEdit4("Specular##", (float *) &color, ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoInputs)) {
                o->setColorSpecular(Color(color.x, color.y, color.z));
            }
            ImGui::SameLine();
            color = {o->ambient.x, o->ambient.y, o->ambient.z, 1};
            if (ImGui::ColorEdit4("Ambient##", (float *) &color, ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoInputs)) {
                o->setAmbient(Color(color.x, color.y, color.z));
            }

            // Effective radius shown as reference (solves attenuation for intensity < 1/256)
            ImGui::Text("Effective radius: %.1f", o->getRadius());
            // Range is the inverse of the preset itself (quadratic = 75/R^2 → R = sqrt(75/quad))
            // so display and preset stay consistent across frames.
            float targetRange;
            if (o->quadratic > 1e-6f)   targetRange = std::sqrt(75.0f / o->quadratic);
            else if (o->linear > 1e-6f) targetRange = 4.5f / o->linear;
            else                        targetRange = 100.0f;
            if (ImGui::DragFloat("Range##rng", &targetRange, 0.5f, 1.0f, 5000.0f, "%.1f")) {
                if (targetRange < 1.0f) targetRange = 1.0f;
                // Standard attenuation preset (LearnOpenGL): const=1, linear=4.5/R, quad=75/R^2
                o->constant  = 1.0f;
                o->linear    = 4.5f  / targetRange;
                o->quadratic = 75.0f / (targetRange * targetRange);
            }

            if (ImGui::TreeNode("Advanced attenuation")) {
                ImGui::SliderFloat("Constant",  &o->constant,  0.1f, 5.0f,  "%.3f");
                ImGui::SliderFloat("Linear",    &o->linear,    0.0f, 2.0f,  "%.5f", ImGuiSliderFlags_Logarithmic);
                ImGui::SliderFloat("Quadratic", &o->quadratic, 0.0f, 5.0f,  "%.6f", ImGuiSliderFlags_Logarithmic);
                if (o->constant  < 0.001f) o->constant  = 0.001f;
                if (o->linear    < 0.0f)   o->linear    = 0.0f;
                if (o->quadratic < 0.0f)   o->quadratic = 0.0f;
                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }
};

#endif //BRAKEZA3D_LIGHTPOINTGUI_H
