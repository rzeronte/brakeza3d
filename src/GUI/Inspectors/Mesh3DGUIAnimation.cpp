//
// Created by Eduardo on 06/12/2025.
//

#include "../../../include/Components/Components.h"
#include "../../../include/GUI/Inspectors/Mesh3DAnimationGUI.h"
#include "../../../include/GUI/Widgets/GUIFields.h"
#include <algorithm>
#include <vector>
#include <string>


void Mesh3DAnimationDrawerGUI::DrawPropertiesGUI(Mesh3DAnimation *o)
{
    constexpr float range_min = 0.0f;
    constexpr float range_max = 1.0f;

    if (ImGui::CollapsingHeader("Mesh3D Animation")) {

        static char animFilter[128] = "";
        {
            GUIFields::Section animations("ANIMATIONS");
            if (animations) {
                GUIFields::String("Filter", animFilter, IM_ARRAYSIZE(animFilter), "Filter animations by name.");
            }
        }

        std::string filterStr = animFilter;
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

        std::vector<int> filtered;
        for (unsigned int i = 0; i < o->scene->mNumAnimations; i++) {
            std::string name = o->scene->mAnimations[i]->mName.C_Str();
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            if (filterStr.empty() || name.find(filterStr) != std::string::npos)
                filtered.push_back((int)i);
        }

        std::vector<const char*> filteredNames;
        int selectedInFiltered = -1;
        for (int fi = 0; fi < (int)filtered.size(); fi++) {
            filteredNames.push_back(o->scene->mAnimations[filtered[fi]]->mName.C_Str());
            if (filtered[fi] == o->indexCurrentAnimation) selectedInFiltered = fi;
        }

        ImGui::SetNextItemWidth(-1);
        if (ImGui::ListBox(("##animlist" + o->getName()).c_str(), &selectedInFiltered, filteredNames.data(), (int)filteredNames.size(), 8)) {
            if (selectedInFiltered >= 0 && selectedInFiltered < (int)filtered.size())
                o->setIndexCurrentAnimation(filtered[selectedInFiltered]);
        }

        {
            GUIFields::Section playback("PLAYBACK");
            if (playback) {
                GUIFields::Float("Speed", &o->animation_speed, 0.01f, range_min, range_max, "%.2f", "Animation playback speed multiplier.");
                GUIFields::Check("Loop", &o->loop);
                GUIFields::Check("Bones Colliders", &o->boneColliderEnabled, "Per-bone colliders defined in the bone mappings.");
            }
        }

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
        static char localVarName[256] = "";
        {
            GUIFields::Section create("CREATE NEW BONES MAPPING");
            if (create) {
                GUIFields::String("Mapping name", localVarName, IM_ARRAYSIZE(localVarName), nullptr, ImGuiInputTextFlags_AutoSelectAll);
            }
        }

        if (ImGui::Button(std::string("Add mapping").c_str())) {
            if (localVarName[0] != '\0') {
                a->createBonesMappingColliders(localVarName);
                localVarName[0] = '\0';
            }
        }

        if (numMappings <= 0) {
            ImGui::End();
            return;
        }

        std::vector<const char*> items;
        items.reserve(bc->size());
        for (const auto& m : *bc) {
            items.push_back(m.nameMapping.c_str());
        }

        {
            GUIFields::Section created("BONE MAPPINGS CREATED");
            if (created) {
                GUIFields::ComboItems("Bone Mappings", &a->BoneColliderIndexPointer(), items);
            }
        }

        if (a->BoneColliderIndexPointer() >= 0) {
            if (ImGui::Button(std::string("Delete mapping").c_str())) {
                a->removeBonesColliderMapping(bc->at(a->BoneColliderIndexPointer()).nameMapping);
                ImGui::End();
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

                for (int i = 0; i < (int)bc->at(a->BoneColliderIndexPointer()).boneColliderInfo.size(); i++) {
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