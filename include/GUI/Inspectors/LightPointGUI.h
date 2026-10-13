//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_LIGHTPOINTGUI_H
#define BRAKEZA3D_LIGHTPOINTGUI_H

#include <cmath>
#include <algorithm>
#include "../../3D/LightPoint.h"
#include "../Widgets/GUIFields.h"

class LightPointGUI
{
public:
    static void DrawPropertiesGUI(LightPoint *o)
    {
        if (ImGui::CollapsingHeader("LightPoint")) {
            ImGui::PushID(o);

            using namespace GUIFields;

            if (BeginSection("COLORS")) {
                float diffuse[3]  = { o->diffuse.x,  o->diffuse.y,  o->diffuse.z };
                float specular[3] = { o->specular.x, o->specular.y, o->specular.z };
                float ambient[3]  = { o->ambient.x,  o->ambient.y,  o->ambient.z };
                if (Color3("Diffuse",  diffuse))  o->setColor(Color(diffuse[0], diffuse[1], diffuse[2]));
                if (Color3("Specular", specular)) o->setColorSpecular(Color(specular[0], specular[1], specular[2]));
                if (Color3("Ambient",  ambient))  o->setAmbient(Color(ambient[0], ambient[1], ambient[2]));
                EndSection();
            }

            if (BeginSection("ATTENUATION")) {
                // Effective radius shown as reference (solves attenuation for intensity < 1/256)
                Text("Effective radius", "%.1f", o->getRadius());
                // Range is the inverse of the preset itself (quadratic = 75/R^2 → R = sqrt(75/quad))
                // so display and preset stay consistent across frames.
                float targetRange;
                if (o->quadratic > 1e-6f)   targetRange = std::sqrt(75.0f / o->quadratic);
                else if (o->linear > 1e-6f) targetRange = 4.5f / o->linear;
                else                        targetRange = 100.0f;
                if (Float("Range", &targetRange, 0.5f, 1.0f, 5000.0f, "%.1f", "Distance at which the light fades out. Sets constant, linear and quadratic from a standard preset.")) {
                    if (targetRange < 1.0f) targetRange = 1.0f;
                    // Standard attenuation preset (LearnOpenGL): const=1, linear=4.5/R, quad=75/R^2
                    o->constant  = 1.0f;
                    o->linear    = 4.5f  / targetRange;
                    o->quadratic = 75.0f / (targetRange * targetRange);
                }
                EndSection();
            }

            if (ImGui::TreeNode("Advanced attenuation")) {
                if (BeginSection("COEFFICIENTS")) {
                    Slider("Constant",  &o->constant,  0.1f, 5.0f, "%.3f");
                    Slider("Linear",    &o->linear,    0.0f, 2.0f, "%.5f", nullptr, ImGuiSliderFlags_Logarithmic);
                    Slider("Quadratic", &o->quadratic, 0.0f, 5.0f, "%.6f", nullptr, ImGuiSliderFlags_Logarithmic);
                    EndSection();
                }
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
