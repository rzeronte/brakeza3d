#ifndef BRAKEZA3D_GUIADDONDOCUMENTATION_H
#define BRAKEZA3D_GUIADDONDOCUMENTATION_H

#include <string>
#include <algorithm>
#include <cctype>
#include "../../Misc/cJSON.h"

class GUIAddonDocumentation {
public:
    std::string currentContent;
    std::string currentFile;

private:
    static char searchBuffer[256];
    static std::string searchTerm;

    // Convierte string a minúsculas para búsqueda case-insensitive
    static std::string ToLower(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return lower;
    }

    // Verifica si un nodo o sus hijos contienen el término de búsqueda
    static bool NodeMatchesSearch(cJSON* node, const std::string& search) {
        if (search.empty()) return true;

        cJSON* nameItem = cJSON_GetObjectItem(node, "name");
        if (!nameItem) return false;

        std::string name = nameItem->valuestring;
        std::string nameLower = ToLower(name);
        std::string searchLower = ToLower(search);

        // Si el nombre contiene el término de búsqueda, coincide
        if (nameLower.find(searchLower) != std::string::npos) {
            return true;
        }

        // Si no coincide, verificar si algún hijo coincide
        cJSON* childrenArray = cJSON_GetObjectItem(node, "children");
        if (childrenArray) {
            int childrenCount = cJSON_GetArraySize(childrenArray);
            for (int i = 0; i < childrenCount; i++) {
                cJSON* child = cJSON_GetArrayItem(childrenArray, i);
                if (NodeMatchesSearch(child, search)) {
                    return true;
                }
            }
        }

        return false;
    }

    // Renderiza un nodo con resaltado opcional
    static void RenderDocNode(cJSON* node, const std::string& search)
    {
        cJSON* nameItem = cJSON_GetObjectItem(node, "name");
        cJSON* urlItem = cJSON_GetObjectItem(node, "url");
        cJSON* childrenArray = cJSON_GetObjectItem(node, "children");

        if (!nameItem) return;

        std::string name = nameItem->valuestring;
        std::string url = urlItem ? urlItem->valuestring : "";

        // Filtrar si no coincide con la búsqueda
        if (!NodeMatchesSearch(node, search)) {
            return;
        }

        int childrenCount = childrenArray ? cJSON_GetArraySize(childrenArray) : 0;
        bool isLeaf = (childrenCount == 0);

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth;

        // Si hay búsqueda activa, expandir automáticamente
        if (!search.empty()) {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));

        if (isLeaf) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            // Resaltar si coincide con la búsqueda
            if (!search.empty() && ToLower(name).find(ToLower(search)) != std::string::npos) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Amarillo
                Drawable::ImGuiLink(name.c_str(), url.c_str());
                ImGui::PopStyleColor();
            } else {
                Drawable::ImGuiLink(name.c_str(), url.c_str());
            }
        } else {
            // Resaltar nombre del nodo padre si coincide
            bool directMatch = !search.empty() &&
                             ToLower(name).find(ToLower(search)) != std::string::npos;

            if (directMatch) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Amarillo
            }

            bool isOpen = ImGui::TreeNodeEx(name.c_str(), flags);

            if (directMatch) {
                ImGui::PopStyleColor();
            }

            if (isOpen) {
                // Renderizar solo hijos que coinciden con la búsqueda
                for (int i = 0; i < childrenCount; i++) {
                    cJSON* child = cJSON_GetArrayItem(childrenArray, i);
                    RenderDocNode(child, search);
                }
                ImGui::TreePop();
            }
        }
        ImGui::PopStyleVar(2);
    }

public:
    static void DrawWinDocumentation(cJSON* tree) {
        if (!tree) return;

        ImGui::Image(FileSystemGUI::Icon(IconGUI::WIN_DOCUMENTATION), GUIType::Sizes::ICON_BROWSER_TYPE);
        ImGui::SameLine();
        ImGui::Text("Documentation");

        ImGui::Separator();

        // Buscador
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
        ImGui::SetNextItemWidth(-1);
        bool searchChanged = ImGui::InputTextWithHint(
            "##DocSearch",
            "Search documentation...",
            searchBuffer,
            IM_ARRAYSIZE(searchBuffer),
            ImGuiInputTextFlags_AutoSelectAll
        );
        ImGui::PopStyleVar();

        if (searchChanged) {
            searchTerm = searchBuffer;
        }

        if (!searchTerm.empty()) {
            ImGui::AlignTextToFramePadding();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            ImGui::Text("Filtering by: '%s'", searchTerm.c_str());
            ImGui::PopStyleColor();
            ImGui::SameLine();
            GUI::DrawButton(
                "Deleting project",
                IconGUI::PROJECT_REMOVE,
                GUIType::Sizes::ICON_LOCKS,
                false,
                [&] {
                    searchBuffer[0] = '\0';
                    searchTerm.clear();
                }
            );
        }

        ImGui::Separator();

        // Árbol de documentación
        ImGui::BeginChild("IndexChild", ImVec2(0, 0), false);
        {
            cJSON* rootArray = cJSON_GetObjectItem(tree, "root");
            if (rootArray) {
                int rootCount = cJSON_GetArraySize(rootArray);

                bool hasResults = false;
                for (int i = 0; i < rootCount; i++) {
                    cJSON* node = cJSON_GetArrayItem(rootArray, i);
                    if (NodeMatchesSearch(node, searchTerm)) {
                        RenderDocNode(node, searchTerm);
                        hasResults = true;
                    }
                }

                // Si hay búsqueda activa pero no hay resultados
                if (!searchTerm.empty() && !hasResults) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
                    ImGui::Text("No results found for '%s'", searchTerm.c_str());
                    ImGui::PopStyleColor();
                }
            }
        }
        ImGui::EndChild();
    }
};

// Inicialización de variables estáticas
char GUIAddonDocumentation::searchBuffer[256] = "";
std::string GUIAddonDocumentation::searchTerm = "";

#endif