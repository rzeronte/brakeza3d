#ifndef BRAKEZA3D_GUIADDONDOCUMENTATION_H
#define BRAKEZA3D_GUIADDONDOCUMENTATION_H

#include <string>
#include "../../Misc/cJSON.h"

class GUIAddonDocumentation {
public:
    std::string currentContent;
    std::string currentFile;
    
    static void RenderDocNode(cJSON* node, TextEditor &editor)
    {
        cJSON* nameItem = cJSON_GetObjectItem(node, "name");
        cJSON* fileItem = cJSON_GetObjectItem(node, "file");
        cJSON* lineItem = cJSON_GetObjectItem(node, "line");
        cJSON* childrenArray = cJSON_GetObjectItem(node, "children");
        
        if (!nameItem) return;
        
        std::string name = nameItem->valuestring;
        std::string file = fileItem ? fileItem->valuestring : "";
        int line = lineItem ? lineItem->valueint : 0;

        int childrenCount = childrenArray ? cJSON_GetArraySize(childrenArray) : 0;
        bool isLeaf = (childrenCount == 0);
        
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
        if (isLeaf) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            ImGui::TreeNodeEx(name.c_str(), flags);
            
            // Click en nodo hoja
            if (ImGui::IsItemClicked()) {
                LOG_MESSAGE("[Documentation] Change to file '%s'...", file.c_str());
                editor.SetText(Tools::ReadFile(file));
                editor.SetCursorPosition({line, 0});
            }
        } else {
            bool isOpen = ImGui::TreeNodeEx(name.c_str(), flags);
            
            // Click en nodo con hijos (opcional, cargar su propio archivo)
            if (ImGui::IsItemClicked() && !file.empty()) {
                LOG_MESSAGE("[Documentation] Change to file '%s'...", file.c_str());
                editor.SetText(Tools::ReadFile(file));
                editor.SetCursorPosition({line, 0});
            }
            
            if (isOpen) {
                // Renderizar hijos recursivamente
                for (int i = 0; i < childrenCount; i++) {
                    cJSON* child = cJSON_GetArrayItem(childrenArray, i);
                    RenderDocNode(child, editor);
                }
                ImGui::TreePop();
            }
        }
        ImGui::PopStyleVar(2);
    }

    static void DrawWinDocumentation(cJSON* tree, TextEditor &editor)
    {
        if (!tree) return;

        ImGui::Image(FileSystemGUI::Icon(IconGUI::WIN_DOCUMENTATION), GUIType::Sizes::ICON_BROWSER_TYPE);

        ImGui::Separator();
        if (ImGui::BeginTable("DocTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV)) {
            // Configurar columnas
            ImGui::TableSetupColumn("Índice", ImGuiTableColumnFlags_WidthFixed, 250.0f);
            ImGui::TableSetupColumn("Contenido", ImGuiTableColumnFlags_WidthStretch);
            
            ImGui::TableNextRow();
            

            // Columna izquierda - Índice del árbol
            ImGui::TableNextColumn();


            ImGui::BeginChild("IndexChild", ImVec2(0, 0), false);
            {
                cJSON* rootArray = cJSON_GetObjectItem(tree, "root");
                if (rootArray) {
                    int rootCount = cJSON_GetArraySize(rootArray);
                    for (int i = 0; i < rootCount; i++) {
                        cJSON* node = cJSON_GetArrayItem(rootArray, i);
                        RenderDocNode(node, editor);
                    }
                }
            }
            ImGui::EndChild();


            // Columna derecha - Contenido del archivo
            ImGui::TableNextColumn();
            ImGui::BeginChild("ContentChild", ImVec2(0, 0), false);
            {
                editor.Render("##doc_editor", ImVec2(0, 0), false);

            }
            ImGui::EndChild();
            
            ImGui::EndTable();
        }
    }
};

#endif