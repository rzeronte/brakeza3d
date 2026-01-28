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
    ImVec2 iconSize = ImVec2(18, 18);
    ImVec2 bulletSize = ImVec2(18, 18);
    ImVec4 iconTint = ImVec4(1, 1, 1, 1);
    ImVec4 selectedColor = ImVec4(0.35f, 0.65f, 0.45f, 0.3f);
    ImVec4 hoveredColor = ImVec4(0.35f, 0.65f, 0.45f, 0.2f);
    float itemPadding = 1.2f;  // Padding vertical de cada item
    float itemMargin = 2.0f;  // Margen vertical entre items
    bool showCheckbox = false;  // Mostrar checkbox opcional
    bool* p_checked = nullptr;  // Puntero al estado del checkbox (true/false)
    float indentSpacing = 20.0f;
    ImVec4 textColor = ImVec4(0, 0, 0, 0);  // (0,0,0,0) means "default color"
    std::function<void()> onDoubleClick = nullptr;
    bool* forceOpenPtr = nullptr;  // Puntero opcional para controlar apertura

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

    // Forzar apertura
    if (g_TreeNodeStates.find(id) == g_TreeNodeStates.end()) {
        g_TreeNodeStates[id] = config.defaultOpen;
    }

    // Aplicar forceOpenPtr directamente al estado
    if (config.forceOpenPtr && *config.forceOpenPtr) {
        g_TreeNodeStates[id] = true;  // Forzar apertura en TU sistema
        *config.forceOpenPtr = false;  // Auto-reset
    }

    if (g_TreeNodeStates.find(id) == g_TreeNodeStates.end()) {
        g_TreeNodeStates[id] = config.defaultOpen;
    }
    bool& is_open = g_TreeNodeStates[id];

    // Reducir espacio entre items para que estén más pegados
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, config.itemMargin));


    // --------------------------------------------------
    // Altura de línea
    // --------------------------------------------------
    const float line_height =
        ImMax(config.iconSize.y, ImGui::GetTextLineHeight()) +
        style.FramePadding.y * config.itemPadding;

    const float text_base_offset_y =
        (line_height - ImGui::GetTextLineHeight()) * 0.5f;

    // --------------------------------------------------
    // Checkbox (fuera)
    // --------------------------------------------------
    float checkbox_width = 0.0f;
    float checkbox_size = 0.0f;

    if (config.showCheckbox && config.p_checked) {
        checkbox_size = ImGui::GetFrameHeight();
        checkbox_width = checkbox_size + style.ItemSpacing.x;
    }

    // --------------------------------------------------
    // Action items (solo para posicionar)
    // --------------------------------------------------
    float action_items_width = 0.0f;
    float action_items_offset_y = 0.0f;

    if (!config.actionItems.empty()) {
        for (const auto& item : config.actionItems)
            action_items_width += item.size.x + style.ItemSpacing.x;

        action_items_width += style.FramePadding.x;
        float button_full_height = config.actionItems[0].size.y + style.FramePadding.y * 2.0f;
        action_items_offset_y = (line_height - button_full_height) * 0.5f;
    }

    // --------------------------------------------------
    // Posiciones base
    // --------------------------------------------------
    ImVec2 full_start = ImGui::GetCursorScreenPos();
    ImVec2 pos_start = full_start;
    pos_start.x += checkbox_width - 4.0f;

    const float full_width = ImGui::GetContentRegionAvail().x;

    // --------------------------------------------------
    // Dibujar CHECKBOX (fuera del invisible)
    // --------------------------------------------------
    if (config.showCheckbox && config.p_checked) {
        ImVec2 cursor_backup = ImGui::GetCursorPos();

        ImVec2 checkbox_pos(
            full_start.x,
            full_start.y + (line_height - checkbox_size) * 0.5f
        );

        ImGui::SetCursorScreenPos(checkbox_pos);
        ImGui::PushID(id + 9999);

        // Reducir el FramePadding para hacer el checkbox más pequeño
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 2.0f)); // Valores más pequeños

        ImGui::Checkbox("##checkbox", config.p_checked);

        ImGui::PopStyleVar(); // Restaurar FramePadding
        ImGui::PopID();
        ImGui::SetCursorPos(cursor_backup);
    }

    // --------------------------------------------------
    // ACTION ITEMS PRIMERO (para que capturen eventos)
    // --------------------------------------------------
    bool action_button_clicked = false;
    bool action_button_hovered = false;

    if (!config.actionItems.empty()) {
        float tree_clickable_width = full_width - checkbox_width;
        ImVec2 buttons_start(
            pos_start.x + tree_clickable_width - action_items_width,
            pos_start.y + action_items_offset_y
        );

        for (size_t i = 0; i < config.actionItems.size(); ++i) {
            const auto& item = config.actionItems[i];

            ImVec2 btn_pos = buttons_start;
            btn_pos.x += i * (item.size.x + style.ItemSpacing.x);

            ImGui::SetCursorScreenPos(btn_pos);
            std::string idItem = std::to_string((int)i + 10000) + config.label;
            ImGui::PushID( idItem.c_str());

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.8f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

            bool clicked = ImGui::ImageButton("##btn", item.icon, item.size);

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);

            if (ImGui::IsItemHovered()) {
                action_button_hovered = true;
            }

            if (clicked) {
                action_button_clicked = true;
                if (item.onClick)
                    item.onClick();
            }

            if (ImGui::IsItemHovered() && !item.tooltip.empty()) {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted(item.tooltip.c_str());
                ImGui::EndTooltip();
            }

            ImGui::PopID();
        }
    }

    // --------------------------------------------------
    // INVISIBLE BUTTON (área del nodo, DESPUÉS de los botones)
    // --------------------------------------------------
    float tree_clickable_width = full_width - checkbox_width;

    ImGui::SetCursorScreenPos(pos_start);
    ImGui::PushID(id);
    bool node_clicked = ImGui::InvisibleButton("##treenode", ImVec2(tree_clickable_width, line_height));
    ImGui::PopID();

    bool hovered = ImGui::IsItemHovered() && !action_button_hovered;

    // Solo procesar click del nodo si no se clickeó un botón de acción
    if (node_clicked && !action_button_clicked && !config.isLeaf)
        is_open = !is_open;

    if (node_clicked && !action_button_clicked && p_selected)
        *p_selected = true;

    // Detectar doble click
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !action_button_clicked) {
        if (config.onDoubleClick)
            config.onDoubleClick();
    }
    // --------------------------------------------------
    // Fondo (cubre TODO salvo checkbox)
    // --------------------------------------------------
    ImU32 bg_col = 0;
    if (p_selected && *p_selected)
        bg_col = ImGui::GetColorU32(config.selectedColor);
    else if (hovered)
        bg_col = ImGui::GetColorU32(config.hoveredColor);

    if (bg_col != 0) {
        window->DrawList->AddRectFilled(
            pos_start,
            ImVec2(pos_start.x + tree_clickable_width, pos_start.y + line_height),
            bg_col
        );
    }

    // --------------------------------------------------
    // Contenido izquierdo
    // --------------------------------------------------
    float x_offset = pos_start.x + style.FramePadding.x;

    // Bullet
    if (!config.isLeaf && (config.bulletOpen || config.bulletClosed)) {
        ImVec2 bullet_pos(
            x_offset,
            pos_start.y + (line_height - config.bulletSize.y) * 0.5f
        );

        ImTextureID tex = is_open ? config.bulletOpen : config.bulletClosed;
        if (tex) {
            ImVec2 bullet_max(
                bullet_pos.x + config.bulletSize.x,
                bullet_pos.y + config.bulletSize.y
            );

            window->DrawList->AddImage(
                tex, bullet_pos, bullet_max,
                ImVec2(0, 0), ImVec2(1, 1),
                ImGui::GetColorU32(config.iconTint)
            );
        }
        x_offset += config.bulletSize.x + 4.0f;
    }

    // Icono izquierdo
    if (config.leftIcon) {
        ImVec2 icon_pos(
            x_offset,
            pos_start.y + (line_height - config.iconSize.y) * 0.5f
        );

        ImVec2 icon_max(
            icon_pos.x + config.iconSize.x,
            icon_pos.y + config.iconSize.y
        );

        window->DrawList->AddImage(
            config.leftIcon,
            icon_pos,
            icon_max,
            ImVec2(0, 0),
            ImVec2(1, 1),
            ImGui::GetColorU32(config.iconTint)
        );

        x_offset += config.iconSize.x + 4.0f;
    }

    // Texto
    std::string display_label = config.label;
    if (config.showChildCount && config.childCount > 0)
        display_label += " (" + std::to_string(config.childCount) + ")";

    // Determinar color del texto
    ImU32 text_color;
    if (config.textColor.w > 0.0f) {  // Si alpha > 0, usar color personalizado
        text_color = ImGui::GetColorU32(config.textColor);
    } else {
        text_color = ImGui::GetColorU32(ImGuiCol_Text);  // Color por defecto
    }

    window->DrawList->AddText(
        ImVec2(x_offset, pos_start.y + text_base_offset_y),
        text_color,  // Usar el color determinado
        display_label.c_str()
    );

    // --------------------------------------------------
    // Drag & drop (misma zona clickable)
    // --------------------------------------------------
    if (config.dragDrop.acceptsDrop && !config.dragDrop.dragDropType.empty()) {
        ImGui::SetCursorScreenPos(pos_start);
        ImGui::InvisibleButton("##dragdrop", ImVec2(tree_clickable_width, line_height));

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload(config.dragDrop.dragDropType.c_str())) {
                if (config.dragDrop.onDropCallback)
                    config.dragDrop.onDropCallback(payload->Data);
            }
            ImGui::EndDragDropTarget();
        }
    }

    if (is_open && !config.isLeaf) {
        if (config.indentSpacing > 0.0f) {
            ImGui::Indent(config.indentSpacing);
        } else {
            ImGui::Indent();
        }
    }

    // Restaurar ItemSpacing
    ImGui::PopStyleVar();

    return is_open && !config.isLeaf;
}

inline void CustomTreePop(float indentSpacing = 0.0f)
{
    if (indentSpacing > 0.0f) {
        ImGui::Unindent(indentSpacing);
    } else {
        ImGui::Unindent();
    }
}

inline void ResetTreeStates() {
    g_TreeNodeStates.clear();
}

} // namespace CustomImGui