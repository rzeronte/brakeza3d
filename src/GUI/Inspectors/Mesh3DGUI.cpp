
#include "../../../include/GUI/Inspectors/Mesh3DGUI.h"
#include "../../../include/GUI/Inspectors/Object3DGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Windows/FileSystemGUI.h"
#include "../../../include/GUI/Widgets/GUIFields.h"

void Mesh3DGUI::DrawPropertiesGUI(Mesh3D *o)
{
    if (ImGui::CollapsingHeader("Mesh3D")) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));
        if (ImGui::TreeNodeEx("Mesh render options", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {
            if (GUIFields::BeginSection("RENDER")) {
                GUIFields::Check("Default pipeline", &o->renderDefaultPipeline, "Render through the deferred pipeline. Off = only the object shaders draw it.");
                GUIFields::Check("Frustum cull submeshes", &o->frustumCullSubmeshes, "Skip submeshes outside the camera view.");
                if (o->hasInstances()) {
                    GUIFields::Text("Instances file", "%s", o->getInstancesFile().c_str());
                    GUIFields::Text("Instances", "%zu  (drawn %zu)", o->getInstanceCount(), o->getVisibleInstanceCount());
                    GUIFields::Text("Chunks", "%zu  (visible %zu)", o->getInstanceChunkCount(), o->getVisibleInstanceChunkCount());
                    float maxDist = o->getInstanceMaxDistance();
                    if (GUIFields::Float("Max distance", &maxDist, 1.0f, 0.0f, 100000.0f, "%.0f", "Chunks farther than this from the camera are not drawn. 0 = no limit."))
                        o->setInstanceMaxDistance(maxDist);
                }
                bool mip = o->isMipmaps();
                if (GUIFields::Check("Mipmaps", &mip, "Mipmaps + trilinear + anisotropic filtering on this model's textures: removes the grainy look at a distance. Textures are shared by path, so it also affects other meshes using them.")) o->setMipmaps(mip);
                GUIFields::Check("Emission", &o->emissionEnabled);
                if (!o->emissionEnabled) ImGui::BeginDisabled();
                GUIFields::Slider("Emission intensity", &o->emissionIntensity, 0.0f, 1.0f);
                if (!o->emissionEnabled) ImGui::EndDisabled();
                GUIFields::EndSection();
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNodeEx("Mesh information", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {
            ImGui::Spacing();
            ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICONS_BROWSERS);
            ImGui::SameLine();
            ImGui::Text(o->sourceFile.c_str());
            ImGui::Spacing();
            // Filtro de texto sin distinguir mayusculas
            auto containsCI = [](const std::string &hay, const char *needle) {
                if (!needle[0]) return true;
                std::string h = hay, n = needle;
                std::transform(h.begin(), h.end(), h.begin(), [](unsigned char c) { return std::tolower(c); });
                std::transform(n.begin(), n.end(), n.begin(), [](unsigned char c) { return std::tolower(c); });
                return h.find(n) != std::string::npos;
            };
            auto textureName = [&](int materialIndex) -> std::string {
                if (materialIndex < 0 || materialIndex >= (int) o->modelTextures.size() || !o->modelTextures[materialIndex]) return "";
                return std::string(o->modelTextures[materialIndex]->getFileName().c_str());
            };

            // ── Submeshes (colapsada; con buscador y lista virtualizada: una ciudad tiene miles) ──
            if (ImGui::TreeNodeEx("Submeshes", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {
                static char subFilter[64] = "";
                static bool subEmissiveOnly = false;
                {
                    GUIFields::Section filter("SUBMESH FILTER");
                    if (filter) {
                        GUIFields::String("Filter", subFilter, sizeof(subFilter), "mesh #, material #, texture...");
                        GUIFields::Check("Emitting only", &subEmissiveOnly);
                    }
                }

                // indices que pasan el filtro
                std::vector<int> rows;
                rows.reserve(o->meshes.size());
                for (int i = 0; i < (int) o->meshes.size(); i++) {
                    const auto &m = o->meshes[i];
                    if (subEmissiveOnly && !(o->emissionEnabled && o->isMaterialEmissive(m.materialIndex))) continue;
                    if (subFilter[0]) {
                        std::string key = "mesh " + std::to_string(i + 1) + " #" + std::to_string(m.materialIndex) + " " + textureName(m.materialIndex);
                        if (!containsCI(key, subFilter)) continue;
                    }
                    rows.push_back(i);
                }
                ImGui::TextDisabled("%d / %d submeshes", (int) rows.size(), (int) o->meshes.size());

                if (ImGui::BeginTable("MeshTable", 9, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_ScrollY, ImVec2(0, 260))) {
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("Mesh");
                    ImGui::TableSetupColumn("Vertices");
                    ImGui::TableSetupColumn("UVs");
                    ImGui::TableSetupColumn("Normals");
                    ImGui::TableSetupColumn("Textures");
                    ImGui::TableSetupColumn("AABB center");
                    ImGui::TableSetupColumn("Visible");
                    ImGui::TableSetupColumn("Material");
                    ImGui::TableSetupColumn("Emits");
                    ImGui::TableHeadersRow();

                    ImGuiListClipper clipper;
                    clipper.Begin((int) rows.size());
                    while (clipper.Step()) {
                        for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; r++) {
                            const int i = rows[r];
                            const auto &m = o->meshes[i];
                            ImGui::TableNextRow();

                            ImGui::TableNextColumn();
                            ImGui::Text("Mesh %d", i + 1);

                            ImGui::TableNextColumn();
                            ImGui::Text("%d", (int) m.vertices.size());

                            ImGui::TableNextColumn();
                            ImGui::Text("%d", (int) m.uvs.size());

                            ImGui::TableNextColumn();
                            ImGui::Text("%d", (int) m.normals.size());

                            ImGui::TableNextColumn();
                            ImGui::Text("%d", (int) o->modelTextures.size());

                            ImGui::TableNextColumn();
                            ImGui::Text("%.0f %.0f %.0f",
                                (m.localAabb.min.x + m.localAabb.max.x) * 0.5f,
                                (m.localAabb.min.y + m.localAabb.max.y) * 0.5f,
                                (m.localAabb.min.z + m.localAabb.max.z) * 0.5f);

                            ImGui::TableNextColumn();
                            if (o->frustumCullSubmeshes) {
                                if (m.visibleInFrustum) ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "YES");
                                else                    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "NO");
                            } else {
                                ImGui::TextDisabled("--");
                            }

                            // Material de la submesh (indice en la lista de materiales) y si emite
                            ImGui::TableNextColumn();
                            ImGui::Text("#%d", m.materialIndex);

                            ImGui::TableNextColumn();
                            if (!o->emissionEnabled)                         ImGui::TextDisabled("--");
                            else if (o->isMaterialEmissive(m.materialIndex)) ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "YES");
                            else                                             ImGui::TextDisabled("no");
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::TreePop();
            }

            // ── Materiales (colapsada; buscador, marcado por nombre y miniaturas pequeñas) ──
            if (ImGui::TreeNodeEx("Materials", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {
                if (!o->emissionEnabled) {
                    ImGui::TextDisabled("Emission is off: enable it in RENDER to make the marked materials glow.");
                } else if (o->emissiveMaterials.empty()) {
                    ImGui::TextWrapped("None marked -> the WHOLE mesh emits. Tick materials to make only their submeshes emit.");
                } else {
                    ImGui::TextWrapped("%d marked -> only submeshes using them emit.", (int) o->emissiveMaterials.size());
                }

                // El mismo texto filtra la lista y es el patron de Mark/Unmark
                static char matFilter[64] = "";
                static bool matEmissiveOnly = false;
                {
                    GUIFields::Section filter("MATERIAL FILTER");
                    if (filter) {
                        GUIFields::String("Filter", matFilter, sizeof(matFilter), "material #, texture name...");
                        GUIFields::Check("Marked only", &matEmissiveOnly);
                    }
                }
                if (ImGui::Button("Mark")) o->setMaterialsEmissiveByTexture(matFilter, true);
                ImGui::SameLine();
                if (ImGui::Button("Unmark")) o->setMaterialsEmissiveByTexture(matFilter, false);
                ImGui::SameLine();
                if (ImGui::Button("Clear all")) o->emissiveMaterials.clear();

                std::vector<int> mats;
                mats.reserve(o->modelTextures.size());
                for (int i = 0; i < (int) o->modelTextures.size(); i++) {
                    if (!o->modelTextures[i]) continue;
                    if (matEmissiveOnly && !o->emissiveMaterials.count(i)) continue;
                    if (matFilter[0] && !containsCI("#" + std::to_string(i) + " " + textureName(i), matFilter)) continue;
                    mats.push_back(i);
                }
                ImGui::TextDisabled("%d / %d materials", (int) mats.size(), (int) o->modelTextures.size());

                if (ImGui::BeginTable("TexturesTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY, ImVec2(0, 260))) {
                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 36.0f);
                    ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, 56.0f);
                    ImGui::TableSetupColumn("Texture");
                    ImGui::TableSetupColumn("Emissive", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                    ImGui::TableHeadersRow();

                    ImGuiListClipper clipper;
                    clipper.Begin((int) mats.size(), 52.0f);
                    while (clipper.Step()) {
                        for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; r++) {
                            const int i = mats[r];
                            auto *t = o->modelTextures[i];
                            ImGui::TableNextRow(0, 52.0f);

                            ImGui::TableNextColumn();
                            ImGui::Text("%d", i);

                            ImGui::TableNextColumn();
                            ImGui::Image(t->getOGLImTexture(), ImVec2(48, 48));

                            ImGui::TableNextColumn();
                            ImGui::TextWrapped("%s", t->getFileName().c_str());

                            ImGui::TableNextColumn();
                            bool em = o->emissiveMaterials.count(i) > 0;
                            ImGui::PushID(i);
                            if (ImGui::Checkbox("##emissive", &em)) o->setMaterialEmissive(i, em);
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::TreePop();
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
                if (GUIFields::BeginSection("NEW GRID")) {
                    GUIFields::SliderInt("Size X", &sizeX, 1, 10);
                    GUIFields::SliderInt("Size Y", &sizeY, 1, 10);
                    GUIFields::SliderInt("Size Z", &sizeZ, 1, 10);
                    GUIFields::EndSection();
                }

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
                if (GUIFields::BeginSection("OCTREE")) {
                    GUIFields::SliderInt("Depth", &maxDepth, 1, 4, "Subdivision levels.");
                    GUIFields::EndSection();
                }
                if (ImGui::Button("Create octree")) {
                    o->BuildOctree(maxDepth);
                }
            }

            if (o->octree != nullptr) {
                static int maxDepth = 1;
                if (GUIFields::BeginSection("OCTREE")) {
                    GUIFields::SliderInt("Depth", &maxDepth, 1, 4, "Subdivision levels.");
                    GUIFields::EndSection();
                }
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
