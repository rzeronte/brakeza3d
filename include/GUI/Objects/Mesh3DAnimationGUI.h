//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_MESH_3D_ANIMATIONGUI_H
#define BRAKEZA3D_MESH_3D_ANIMATIONGUI_H

#include "../../3D/Mesh3DAnimation.h"
#include "../../Brakeza.h"

class Mesh3DAnimationDrawerGUI
{
public:
    static void DrawPropertiesGUI(Mesh3DAnimation *o)
    {
        const float range_min = 0.0f;
        const float range_max = 1.0f;

        if (ImGui::CollapsingHeader("Mesh 3D")) {

            const char* items[o->scene->mNumAnimations];
            for (unsigned int i = 0; i < o->scene->mNumAnimations; i++) {
                items[i] = o->scene->mAnimations[i]->mName.C_Str();
            }
            auto comboTitle = "Animations##" + o->getLabel();
            ImGui::Combo("Animation", &o->indexCurrentAnimation, items, IM_ARRAYSIZE(items));

            ImGui::Separator();
            ImGui::DragScalar("Speed", ImGuiDataType_Float, &o->animation_speed, 0.01f ,&range_min, &range_max, "%f", 1.0f);
            ImGui::Separator();
            ImGui::Checkbox("Loop", &o->loop);
            ImGui::Separator();
            ImGui::Checkbox("Bones Colliders", &o->boneColliderEnabled);

            if (o->boneColliderEnabled) {
                if (ImGui::Button(std::string("Manage Bone Mappings##" + o->getLabel()).c_str())) {
                   Brakeza::get()->GUI()->OpenBoneInfoDialog();
                }
            }
        }
    }
};

#endif //BRAKEZA3D_MESH_3D_ANIMATIONGUI_H