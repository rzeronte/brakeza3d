//
// Created by Eduardo on 06/12/2025.
//

#include "../../../include/Components/Components.h"
#include "../../../include/GUI/Objects/Mesh3DAnimationGUI.h"


void Mesh3DAnimationDrawerGUI::DrawPropertiesGUI(Mesh3DAnimation *o)
{
    constexpr float range_min = 0.0f;
    constexpr float range_max = 1.0f;

    if (ImGui::CollapsingHeader("Mesh3D Animation")) {

        const char* items[o->scene->mNumAnimations];
        for (unsigned int i = 0; i < o->scene->mNumAnimations; i++) {
            items[i] = o->scene->mAnimations[i]->mName.C_Str();
        }
        auto comboTitle = "Animations##" + o->getName();
        ImGui::Combo("Animation", &o->indexCurrentAnimation, items, IM_ARRAYSIZE(items));

        ImGui::Separator();
        ImGui::DragScalar("Speed", ImGuiDataType_Float, &o->animation_speed, 0.01f ,&range_min, &range_max, "%f", 1.0f);
        ImGui::Separator();
        ImGui::Checkbox("Loop", &o->loop);
        ImGui::Separator();
        ImGui::Checkbox("Bones Colliders", &o->boneColliderEnabled);

        if (o->boneColliderEnabled) {
            if (ImGui::Button(std::string("Manage Bone Mappings##" + o->getName()).c_str())) {
                Brakeza::get()->GUI()->OpenBoneInfoDialog();
            }
        }
    }
}
void Mesh3DAnimationDrawerGUI::DrawEditBonesMappingWindow(GUIManager *gui)
{
    if (!gui->showBoneMappingsEditorWindow) return;

    auto o = Components::get()->Render()->getSelectedObject();
    if ( o == nullptr) return;

    auto a = dynamic_cast<Mesh3DAnimation*>(o);
    if (a == nullptr) return;

    auto bc = a->getBoneMappingColliders();
    int numMappings = (int) bc->size();

    GUIManager::SetNextWindowSize(700, 700);
    ImGui::SetNextWindowBgAlpha(GUIType::Levels::WINDOW_ALPHA);
    auto dialogTitle = std::string("Bones Mapping Editor: " + a->getName());
    if (ImGui::Begin(dialogTitle.c_str(), &gui->showBoneMappingsEditorWindow, ImGuiWindowFlags_NoDocking)) {
        ImGui::SeparatorText("Create new bones mapping:");

        static char localVarName[256] = "";
        ImGui::InputText("Mapping name", localVarName, IM_ARRAYSIZE(localVarName), ImGuiInputTextFlags_AutoSelectAll);

        ImGui::SameLine();
        if (ImGui::Button(std::string("Add mapping").c_str())) {
            if (localVarName[0] != '\0') {
                a->createBonesMappingColliders(localVarName);
                localVarName[0] = '\0';
            }
        }

        if (numMappings <= 0) return;

        ImGui::SeparatorText("Bone mappings created");

        const char* items[numMappings];
        for (unsigned int i = 0; i < bc->size(); i++) {
            items[i] = bc->at(i).nameMapping.c_str();
        }

        auto comboTitle = "BoneMappings##" + a->getName();
        ImGui::Combo("Bone Mappings", &a->BoneColliderIndexPointer(), items, IM_ARRAYSIZE(items));

        if (a->BoneColliderIndexPointer() >= 0) {
            if (ImGui::Button(std::string("Delete mapping").c_str())) {
                a->removeBonesColliderMapping(bc->at(a->BoneColliderIndexPointer()).nameMapping);
                return;
            }
        }

        if (a->BoneColliderIndexPointer() != -1) {
            ImGui::SeparatorText(std::string("List bones for mapping: " + bc->at(a->BoneColliderIndexPointer()).nameMapping).c_str());

            auto nameMapping = bc->at(a->BoneColliderIndexPointer()).nameMapping;
            if (ImGui::BeginTable("BoneMappingTable", 4, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Index");
                ImGui::TableSetupColumn("Bone Name");
                ImGui::TableSetupColumn("Bone ID");
                ImGui::TableSetupColumn("Enabled");
                ImGui::TableHeadersRow();

                for (int i = 0; i < bc->at(a->BoneColliderIndexPointer()).boneColliderInfo.size(); i++) {
                    auto& b = bc->at(a->BoneColliderIndexPointer()).boneColliderInfo[i];

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", i + 1);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", b.name.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d", b.boneId);

                    ImGui::TableSetColumnIndex(3);

                    GUI::DrawButton(
                        std::string((b.enabled ? "lock##" : "unlock##")) + std::to_string(i),
                        b.enabled ? IconGUI::BONE_LOCK : IconGUI::BONE_UNLOCK,
                        GUIType::Sizes::ICON_LOCKS,
                        true,
                        [&] {
                        a->SetMappingBoneColliderInfo(nameMapping, b.boneId, !b.enabled, b.shape);
                    });
                }
                ImGui::EndTable();
            }
        }
    }

    ImGui::End();
}