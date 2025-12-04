//
// Created by Eduardo on 04/12/2025.
//

#include "../../include/GUI/GUI.h"

#include "../../include/Misc/Tools.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"

void GUI::DrawButton(const std::string &tooltip, GUISheet icon, bool active, const std::function<void()>& onClick)
{
    const auto id = tooltip + std::string(icon.x + "_" + icon.y);
    ImGui::PushID(id.c_str());
    ImGui::PushStyleColor(ImGuiCol_Button, active ? GUITypes::Colors::BTN_ON : GUITypes::Colors::BTN_OFF);
    if (ImGui::ImageButton(FileSystemGUI::Icon(icon), GUITypes::Sizes::ICON_SIZE_MENUS)) {
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

GUITypes::FolderBrowserCache GUI::CreateBrowserCache(std::string folder, std::string &extension)
{
    GUITypes::FolderBrowserCache cache = {
        folder,
        Tools::getFolderFolders(folder),
        Tools::getFolderFiles(folder, extension)
    };

    return cache;
}

