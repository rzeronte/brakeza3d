//
// Created by Eduardo on 04/01/2026.
//

#include "../../../include/GUI/Editable/EditableOpenFileGUI.h"

#include "../../../include/GUI/Editable/EditableOpenFile.h"
#include "../../../include/Brakeza.h"


void EditableOpenFileGUI::DrawCodeEditorTab(EditableOpenFile &file, int tabIndex)
{
    std::string uniqueTabId = file.getTabLabel() + "##" + file.getPath();

    auto currentIndexTab = Brakeza::get()->GUI()->getIndexCodeEditorTab();

    ImGuiTabItemFlags flags = (currentIndexTab == tabIndex) ? ImGuiTabItemFlags_SetSelected : 0;
    if (ImGui::BeginTabItem(uniqueTabId.c_str(), nullptr, flags)) {
        static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
        std::string tableId = "codeEditorTab##" + file.getPath();
        if (ImGui::BeginTable(tableId.c_str(), 2, flags)) {
            ImGui::TableSetupColumn("Setup", ImGuiTableColumnFlags_WidthFixed, 350.0f);
            ImGui::TableSetupColumn("Code", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            // Config File
            ImGui::TableSetColumnIndex(0);
            file.DrawEditableOpenFileConfig();
            // Code Editor
            ImGui::TableSetColumnIndex(1);
            file.DrawCodeEditActionButtons();
            file.getEditor().Render("##editor", ImVec2(0, 0), false);
            ImGui::EndTable();
        }
        ImGui::EndTabItem();
    }
}
