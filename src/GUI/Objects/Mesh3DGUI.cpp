
#include "../../../include/GUI/Objects/Mesh3DGUI.h"
#include "../../../include/GUI/Objects/Object3DGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"

void Mesh3DGUI::DrawPropertiesGUI(Mesh3D *o)
{
    if (ImGui::CollapsingHeader("Mesh3D")) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));
        if (ImGui::TreeNodeEx("Mesh render options", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {
            ImGui::Checkbox("Render default pipeline", &o->renderDefaultPipeline);
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNodeEx("Mesh information", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {
            ImGui::Spacing();
            ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_BROWSERS);
            ImGui::SameLine();
            ImGui::Text(o->sourceFile.c_str());
            ImGui::Spacing();
            if (ImGui::BeginTable("MeshTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame)) {
                // Header
                ImGui::TableSetupColumn("Mesh");
                ImGui::TableSetupColumn("Vertices");
                ImGui::TableSetupColumn("UVs");
                ImGui::TableSetupColumn("Normals");
                ImGui::TableSetupColumn("Textures");
                ImGui::TableHeadersRow();

                // Rows
                int cont = 1;
                for (auto &m: o->meshes) {
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text("Mesh %d", cont);

                    ImGui::TableNextColumn();
                    ImGui::Text("%d", (int) m.vertices.size());

                    ImGui::TableNextColumn();
                    ImGui::Text("%d", (int) m.uvs.size());

                    ImGui::TableNextColumn();
                    ImGui::Text("%d", (int) m.normals.size());

                    ImGui::TableNextColumn();
                    ImGui::Text("%d", (int) o->modelTextures.size());

                    cont++;
                }

                ImGui::EndTable();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::BeginTable("TexturesTable", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Textures");
                ImGui::TableHeadersRow();

                for (auto &m : o->modelTextures) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    float fixedWidth = std::min((int) ImGui::GetContentRegionAvail().x, m->width());
                    float height = fixedWidth * ((float) m->height() / (float) m->width());
                    ImGui::Image(m->getOGLImTexture(), ImVec2(fixedWidth, height));
                }

                ImGui::EndTable();
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNodeEx("Grid3D", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {
            ImGui::Spacing();
            if (o->grid != nullptr) {
                if (ImGui::Button("Fill from mesh geometry")) {
                    o->FillGrid3DFromGeometry();
                }
                o->grid->DrawImGuiProperties();
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
                    o->BuildGrid3D(sizeX, sizeY, sizeZ);
                }
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNodeEx("Octree", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {
            ImGui::Spacing();
            if (o->octree == nullptr) {
                static int maxDepth = 1;
                ImGui::SliderInt("Depth", &maxDepth, 1, 4);
                if (ImGui::Button("Create octree")) {
                    o->BuildOctree(maxDepth);
                }
            }

            if (o->octree != nullptr) {
                static int maxDepth = 1;
                ImGui::SliderInt("Depth", &maxDepth, 1, 4);
                if (ImGui::Button("Update octree")) {
                    o->BuildOctree(maxDepth);
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete Octree")) {
                    delete o->octree;
                    o->octree = nullptr;
                }
            }

            ImGui::TreePop();
        }
        ImGui::PopStyleVar(2);
    }
}
