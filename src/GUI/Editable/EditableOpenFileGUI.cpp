//
// Created by Eduardo on 04/01/2026.
//

#include "../../../include/GUI/Editable/EditableOpenFileGUI.h"

#include "../../../include/GUI/Editable/EditableOpenFile.h"
#include "../../../include/Brakeza.h"


void EditableOpenFileGUI::DrawCodeEditorTab(EditableOpenFile &file, int tabIndex)
{
    std::string uniqueTabId = file.getTabLabel() + "##" + file.getPath();

    if (file.getWinType() == TAB) {
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

    if (file.getWinType() == POPUP) {
        // Obtener el tamaño del viewport
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 center = viewport->GetCenter();

        // Establecer tamaño grande (80% del viewport)
        ImVec2 windowSize = ImVec2(viewport->Size.x * 0.8f, viewport->Size.y * 0.8f);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);
        // Centrar la ventana
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        // Crear popup modal
        if (ImGui::BeginPopupModal(uniqueTabId.c_str(), nullptr, ImGuiWindowFlags_NoCollapse)) {
            static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
            std::string tableId = "codeEditorPopup##" + file.getPath();

            // Calcular altura disponible (dejando espacio para el botón cerrar)
            float availableHeight = ImGui::GetContentRegionAvail().y - 40.0f;
            if (ImGui::BeginTable(tableId.c_str(), 2, flags, ImVec2(0, availableHeight))) {
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
            // Botón para cerrar
            ImGui::Separator();
            if (ImGui::Button("Cerrar", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        // Abrir el popup (solo la primera vez)
        if (!ImGui::IsPopupOpen(uniqueTabId.c_str())) {
            ImGui::OpenPopup(uniqueTabId.c_str());
        }
    }
}