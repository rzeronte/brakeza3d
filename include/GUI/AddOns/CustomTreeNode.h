#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace CustomImGui {

struct TreeActionItem {
    ImTextureID icon;
    std::string tooltip;
    std::function<void()> onClick;
    ImVec2 size = ImVec2(16, 16);

    TreeActionItem(ImTextureID icon, const std::string& tooltip, std::function<void()> callback)
        : icon(icon), tooltip(tooltip), onClick(callback) {}
};

struct TreeDragDropConfig {
    bool acceptsDrop = false;
    std::string dragDropType;
    std::function<void(void*)> onDropCallback;

    TreeDragDropConfig() = default;
    TreeDragDropConfig(const std::string& type, std::function<void(void*)> callback)
        : acceptsDrop(true), dragDropType(type), onDropCallback(callback) {}
};

struct CustomTreeNodeConfig {
    const char* label;
    ImTextureID leftIcon = nullptr;
    ImTextureID bulletOpen = nullptr;
    ImTextureID bulletClosed = nullptr;
    std::vector<TreeActionItem> actionItems;
    TreeDragDropConfig dragDrop;
    bool showChildCount = false;
    int childCount = 0;
    bool isLeaf = false;
    bool defaultOpen = false;
    ImVec2 iconSize = ImVec2(16, 16);
    ImVec2 bulletSize = ImVec2(12, 12);
    ImVec4 iconTint = ImVec4(1, 1, 1, 1);
    ImVec4 selectedColor = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    ImVec4 hoveredColor = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    float itemPadding = 0.7f;  // Padding vertical de cada item (multiplicador de FramePadding.y)

    CustomTreeNodeConfig(const char* label) : label(label) {}
};

static std::unordered_map<ImGuiID, bool> g_TreeNodeStates;

inline bool CustomTreeNode(CustomTreeNodeConfig& config, bool* p_selected = nullptr) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = ImGui::GetStyle();
    const ImGuiID id = window->GetID(config.label);

    if (g_TreeNodeStates.find(id) == g_TreeNodeStates.end()) {
        g_TreeNodeStates[id] = config.defaultOpen;
    }
    bool& is_open = g_TreeNodeStates[id];

    const float line_height = ImMax(config.iconSize.y, ImGui::GetTextLineHeight()) + style.FramePadding.y * config.itemPadding;  // Usa config.itemPadding
    const float text_base_offset_y = ImMax(0.0f, (line_height - ImGui::GetTextLineHeight()) * 0.5f);

    // Calcular offset para action items - ALTURA REAL del ImageButton
    float action_items_offset_y = 0.0f;
    if (!config.actionItems.empty()) {
        // ImageButton añade FramePadding, así que la altura REAL es:
        float button_full_height = config.actionItems[0].size.y + style.FramePadding.y * 2.0f;
        action_items_offset_y = (line_height - button_full_height) * 0.5f;
    }

    float action_items_width = 0.0f;
    if (!config.actionItems.empty()) {
        for (const auto& action_item : config.actionItems) {
            action_items_width += action_item.size.x + style.ItemSpacing.x;
        }
        // Añadir margen extra a la derecha del último botón
        action_items_width += style.FramePadding.x * 3.0f;  // Aumentado para más margen derecho
    }

    ImVec2 pos_start = ImGui::GetCursorScreenPos();
    const float full_width = ImGui::GetContentRegionAvail().x;

    std::string display_label = config.label;
    if (config.showChildCount && config.childCount > 0) {
        display_label += " (" + std::to_string(config.childCount) + ")";
    }

    // ====================
    // ÁREA CLICKABLE DEL TREENODE (sin incluir botones)
    // ====================
    float tree_clickable_width = full_width - action_items_width - style.ItemSpacing.x;

    ImGui::PushID(id);
    bool node_clicked = ImGui::InvisibleButton("##treenode", ImVec2(tree_clickable_width, line_height));
    ImGui::PopID();

    bool hovered = ImGui::IsItemHovered();

    // Click para toggle/selección
    if (node_clicked && !config.isLeaf) {
        is_open = !is_open;
    }

    if (node_clicked && p_selected) {
        *p_selected = true;
    }

    // Fondo
    ImVec2 pos_end = ImVec2(pos_start.x + full_width, pos_start.y + line_height);
    ImRect full_rect(pos_start, pos_end);

    ImU32 bg_col = 0;
    if (p_selected && *p_selected) {
        bg_col = ImGui::GetColorU32(config.selectedColor);
    } else if (hovered) {
        bg_col = ImGui::GetColorU32(config.hoveredColor);
    }

    if (bg_col != 0) {
        window->DrawList->AddRectFilled(full_rect.Min, full_rect.Max, bg_col);
    }

    // ====================
    // DIBUJAR ELEMENTOS
    // ====================
    float x_offset = pos_start.x + style.FramePadding.x;

    // 1. BULLET
    if (!config.isLeaf && (config.bulletOpen || config.bulletClosed)) {
        ImVec2 bullet_pos(x_offset, pos_start.y + (line_height - config.bulletSize.y) * 0.5f);
        ImTextureID bullet_tex = is_open ? config.bulletOpen : config.bulletClosed;
        if (bullet_tex) {
            window->DrawList->AddImage(bullet_tex, bullet_pos,
                ImVec2(bullet_pos.x + config.bulletSize.x, bullet_pos.y + config.bulletSize.y),
                ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(config.iconTint));
        }
        x_offset += config.bulletSize.x + 4.0f;
    } else if (!config.isLeaf) {
        x_offset += config.bulletSize.x + 4.0f;
    }

    // 2. ICONO IZQUIERDO
    if (config.leftIcon) {
        ImVec2 icon_pos(x_offset, pos_start.y + (line_height - config.iconSize.y) * 0.5f);
        window->DrawList->AddImage(config.leftIcon, icon_pos,
            ImVec2(icon_pos.x + config.iconSize.x, icon_pos.y + config.iconSize.y),
            ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(config.iconTint));
        x_offset += config.iconSize.x + 4.0f;
    }

    // 3. TEXTO
    window->DrawList->AddText(ImVec2(x_offset, pos_start.y + text_base_offset_y),
                              ImGui::GetColorU32(ImGuiCol_Text), display_label.c_str());

    // ====================
    // 4. ACTION ITEMS - CLICKABLES
    // ====================
    if (!config.actionItems.empty()) {
        // Guardar posición actual del cursor
        ImVec2 cursor_backup = ImGui::GetCursorPos();

        // Calcular posición ABSOLUTA en pantalla para los botones
        ImVec2 buttons_start_screen = pos_start;
        buttons_start_screen.x += full_width - action_items_width + style.ItemSpacing.x;
        buttons_start_screen.y += action_items_offset_y;  // Usar el offset calculado (mismo método que el texto)

        // Dibujar cada botón en su posición absoluta
        for (size_t i = 0; i < config.actionItems.size(); ++i) {
            const auto& action_item = config.actionItems[i];  // Cambio: item → action_item

            // Calcular posición de este botón
            ImVec2 button_pos = buttons_start_screen;
            button_pos.x += i * (action_item.size.x + style.ItemSpacing.x);

            // Establecer cursor en posición absoluta (screen coordinates)
            ImGui::SetCursorScreenPos(button_pos);

            ImGui::PushID((int)i);

            // Botón con estilos
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.8f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

            bool clicked = ImGui::ImageButton("##btn", action_item.icon, action_item.size);

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);

            // EJECUTAR CALLBACK
            if (clicked && action_item.onClick) {
                action_item.onClick();
            }

            // Tooltip
            if (ImGui::IsItemHovered() && !action_item.tooltip.empty()) {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted(action_item.tooltip.c_str());
                ImGui::EndTooltip();
            }

            ImGui::PopID();
        }

        // Restaurar cursor a su posición original (relativa a la ventana)
        ImGui::SetCursorPos(cursor_backup);
    }

    // Drag & drop
    if (config.dragDrop.acceptsDrop && !config.dragDrop.dragDropType.empty()) {
        ImGui::SetCursorScreenPos(pos_start);
        ImGui::InvisibleButton("##dragdrop", ImVec2(full_width, line_height));
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(config.dragDrop.dragDropType.c_str())) {
                if (config.dragDrop.onDropCallback) {
                    config.dragDrop.onDropCallback(payload->Data);
                }
            }
            ImGui::EndDragDropTarget();
        }
    }

    // Indent si está abierto
    if (is_open && !config.isLeaf) {
        ImGui::Indent();
    }

    return is_open && !config.isLeaf;
}

inline void CustomTreePop() {
    ImGui::Unindent();
}

inline void ResetTreeStates() {
    g_TreeNodeStates.clear();
}

} // namespace CustomImGui