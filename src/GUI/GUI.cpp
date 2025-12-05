//
// Created by Eduardo on 04/12/2025.
//

#include "../../include/GUI/GUI.h"

#include "../../include/Misc/Tools.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"


// ========== ICON OBJECT DEFINITIONS ==========
namespace IconObject {
    // Definir las variables editables con valores iniciales
#define DEFINE_ICON(name, x, y) GUIType::Sheet name = {x, y};
    ICON_OBJECT_LIST(DEFINE_ICON)
    #undef DEFINE_ICON

    // Array de metadatos para el editor
    GUIType::IconEntry ALL_ICONS_EDITOR[ICON_COUNT] = {
#define MAKE_ENTRY(name, x, y) {#name, &name, {x, y}},
        ICON_OBJECT_LIST(MAKE_ENTRY)
        #undef MAKE_ENTRY
    };

    // Función para resetear a valores originales
    void ResetToDefault() {
        for (size_t i = 0; i < ICON_COUNT; i++) {
            *ALL_ICONS_EDITOR[i].icon = ALL_ICONS_EDITOR[i].original;
        }
    }
}

// ========== ICON GUI DEFINITIONS ==========
namespace IconGUI {
    // Definir las variables editables con valores iniciales
#define DEFINE_ICON(name, x, y) GUIType::Sheet name = {x, y};
    ICON_GUI_LIST(DEFINE_ICON)
    #undef DEFINE_ICON

    // Array de metadatos para el editor
    GUIType::IconEntry ALL_ICONS_EDITOR[ICON_COUNT] = {
#define MAKE_ENTRY(name, x, y) {#name, &name, {x, y}},
        ICON_GUI_LIST(MAKE_ENTRY)
        #undef MAKE_ENTRY
    };

    // Función para resetear a valores originales
    void ResetToDefault() {
        for (size_t i = 0; i < ICON_COUNT; i++) {
            *ALL_ICONS_EDITOR[i].icon = ALL_ICONS_EDITOR[i].original;
        }
    }
}

void GUI::DrawButton(const std::string &tooltip, GUIType::Sheet icon, ImVec2 size, bool active, const std::function<void()>& onClick)
{
    const auto id = tooltip + std::to_string(icon.x) + "_" + std::to_string(icon.y);
    ImGui::PushID(id.c_str());
    ImGui::PushStyleColor(ImGuiCol_Button, active ? GUIType::Colors::BTN_ON : GUIType::Colors::BTN_OFF);
    if (ImGui::ImageButton(FileSystemGUI::Icon(icon), size)) {
        onClick();
    }
    ImGui::PopStyleColor();
    ImGui::PopID();
    ImGui::SetItemTooltip(tooltip.c_str());
}

void GUI::Toggle(bool &value)
{
    value = !value;
}

GUIType::FolderBrowserCache GUI::CreateBrowserCache(std::string folder, const std::string &extension)
{
    GUIType::FolderBrowserCache cache = {
        folder,
        Tools::getFolderFiles(folder, extension),
        Tools::getFolderFolders(folder)
    };

    return cache;
}

