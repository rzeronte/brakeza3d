
#include "../../../include/GUI/Objects/Mesh3DGUI.h"
#include "../../../include/GUI/GUIManager.h"

void Mesh3DGUI::DrawPropertiesGUI(Mesh3D *o)
{
    std::string title = "Mesh3D (File: " + o->sourceFile + ")";

    if (ImGui::CollapsingHeader("Mesh3D")) {
        if (ImGui::TreeNode("Lights in object")) {
            ImGui::Checkbox(std::string("Enable lights for this object").c_str(), &o->enableLights);
            ImGui::TreePop();
        }
        ImGui::Separator();
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
                    ImGui::Text("Num Textures: %d", o->modelTextures.size());
                    ImGui::TreePop();
                }
                cont++;
            }
            if (ImGui::TreeNode("Textures")) {
                for (auto &m : o->modelTextures) {
                    float fixedWidth = std::min((int) ImGui::GetContentRegionAvail().x, m->width());
                    float height = fixedWidth * ((float) m->height() / (float) m->width());
                    ImGui::Image(m->getOGLImTexture(), ImVec2(fixedWidth, height));
                    ImGui::NewLine();
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNode("Grid3D")) {
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
        if (ImGui::TreeNode("Octree")) {
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

    }
}