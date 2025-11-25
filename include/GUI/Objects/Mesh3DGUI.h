//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../3D/Mesh3D.h"

class Mesh3DGUI
{
public:
    static void drawImGuiProperties(Mesh3D *o)
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
};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H