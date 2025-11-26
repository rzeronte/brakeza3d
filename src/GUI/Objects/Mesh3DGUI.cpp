
#include "../../../include/GUI/Objects/Mesh3DGUI.h"
#include "../../../include/GUI/GUIManager.h"

void Mesh3DGUI::drawImGuiProperties(Mesh3D *o)
{
    std::string title = "Mesh3D (File: " + o->sourceFile + ")";

    if (ImGui::CollapsingHeader("Mesh3D")) {
        if (ImGui::TreeNode("Mesh information")) {
            auto fileModel = std::string("- File model: ") + o->sourceFile;
            ImGui::Text(fileModel.c_str());
            ImGui::Text("- Num Meshes: %d", o->meshes.size());
            int cont = 1;
            for (auto &m: o->meshes) {
                auto meshTitle = "Mesh " + std::to_string(cont);
                if (ImGui::TreeNode(meshTitle.c_str())) {
                    ImGui::Text("Num Vertices: %d", m.vertices.size());
                    ImGui::Text("Num UVs: %d", m.uvs.size());
                    ImGui::Text("Num Normals: %d", m.normals.size());
                    ImGui::TreePop();
                }
                cont++;
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNode("Grid3D")) {
            if (o->grid != nullptr) {
                if (ImGui::Button("Fill from mesh geometry")) {
                    o->fillGrid3DFromGeometry();
                }
                o->grid->drawImGuiProperties();
                ImGui::SameLine();
                if (ImGui::Button("Delete Grid3D")) {
                    delete o->grid;
                    o->grid = nullptr;
                }
            } else {
                static int sizeX = 1;
                static int sizeY = 1;
                static int sizeZ = 1;
                ImGui::SliderInt("Size X", &sizeX, 1, 10);
                ImGui::SliderInt("Size Y", &sizeY, 1, 10);
                ImGui::SliderInt("Size Z", &sizeZ, 1, 10);

                if (ImGui::Button("Create Grid3D")) {
                    o->buildGrid3D(sizeX, sizeY, sizeZ);
                }
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNode("Octree")) {
            if (o->octree == nullptr) {
                static int maxDepth = 1;
                ImGui::SliderInt("Depth", &maxDepth, 1, 4);
                if (ImGui::Button("Create octree")) {
                    o->buildOctree(maxDepth);
                }
            }

            if (o->octree != nullptr) {
                static int maxDepth = 1;
                ImGui::SliderInt("Depth", &maxDepth, 1, 4);
                if (ImGui::Button("Update octree")) {
                    o->buildOctree(maxDepth);
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete Octree")) {
                    delete o->octree;
                    o->octree = nullptr;
                }
            }

            ImGui::TreePop();
        }
        ImGui::Separator();
        ImGui::Checkbox(std::string("Enable lights").c_str(), &o->enableLights);
    }
}

void Mesh3DGUI::DrawEditBonesMappingWindow(GUIManager *gui)
{
    if (gui->selectedObjectIndex < 0) return;
    if (!gui->showBoneMappingsEditorWindow) return;

    auto a = dynamic_cast<Mesh3DAnimation*>(gui->gameObjects[gui->selectedObjectIndex]);

    if (a == nullptr) return;

    auto bc = a->getBoneMappingColliders();
    int numMappings = bc->size();

    gui->setNextWindowSize(700, 700);
    ImGui::SetNextWindowBgAlpha(GUIConstants::WINDOW_ALPHA);
    auto dialogTitle = std::string("Bones Mapping Editor: " + a->getLabel());
    if (ImGui::Begin(dialogTitle.c_str(), &gui->showBoneMappingsEditorWindow, ImGuiWindowFlags_NoDocking)) {
        ImGui::SeparatorText("Create new bones mapping:");

        static char name[256];
        strncpy(name, gui->currentVariableToAddName.c_str(), sizeof(name));
        if (ImGui::InputText("Mapping name", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
            gui->currentVariableToAddName = name;
        }
        ImGui::SameLine();
        if (ImGui::Button(std::string("Add mapping").c_str())) {
            if (!gui->currentVariableToAddName.empty()) {
                a->createBonesMappingColliders(gui->currentVariableToAddName);
                gui->currentVariableToAddName.clear();
            }
        }
        if (numMappings <= 0) return;

        ImGui::SeparatorText("Bone mappings created");

        if (numMappings > 0) {
            const char* items[numMappings];
            for (unsigned int i = 0; i < bc->size(); i++) {
                items[i] = bc->at(i).nameMapping.c_str();
            }

            auto comboTitle = "BoneMappings##" + a->getLabel();
            ImGui::Combo("Bone Mappings", &a->BoneColliderIndexPointer(), items, IM_ARRAYSIZE(items));

            if (a->BoneColliderIndexPointer() >= 0) {
                if (ImGui::Button(std::string("Delete mapping").c_str())) {
                    a->removeBonesColliderMapping(bc->at(a->BoneColliderIndexPointer()).nameMapping);
                    return;
                }
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
                    if (ImGui::ImageButton((std::string((b.enabled ? "lock##" : "unlock##")) + std::to_string(i)).c_str(),
                                           TexturePackage::getOGLTextureID(gui->icons, b.enabled ? "lockIcon" : "unlockIcon"),
                                           ImVec2(14, 14))) {
                        a->SetMappingBoneColliderInfo(nameMapping, b.boneId, !b.enabled, b.shape);
                    }
                }
                ImGui::EndTable();
            }
        }
    }

    ImGui::End();
}
