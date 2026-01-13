//
// Created by Eduardo on 04/01/2026.
//

#include "../../../include/GUI/Editable/EditableOpenNode.h"

#include "../../../include/Brakeza.h"
#include "../../../include/Misc/Tools.h"


EditableOpenNode::EditableOpenNode(const std::string &tabLabel, const std::string &path, ShaderBaseNodes *shaderNodes)
:
    EditableOpenBaseResource(tabLabel, path),
    shaderNodes(shaderNodes)
{
}

void EditableOpenNode::DrawTabEdition(int tabIndex)
{
    std::string uniqueTabId = getTabLabel() + "##" + getPath();

    if (getWinType() == TAB) {
        auto currentIndexTab = Brakeza::get()->GUI()->getIndexCodeEditorTab();

        ImGuiTabItemFlags flags = (currentIndexTab == tabIndex) ? ImGuiTabItemFlags_SetSelected : 0;
        if (ImGui::BeginTabItem(uniqueTabId.c_str(), nullptr, flags)) {
            DrawCodeEditActionButtons();
            DrawNodeEditorTab();
            ImGui::EndTabItem();
        }
    }

    if (getWinType() == POPUP) {
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
            DrawCodeEditActionButtons();
            DrawNodeEditorTab();
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

void EditableOpenNode::DrawEditableOpenNodeConfig()
{

}

void EditableOpenNode::DrawNodeEditorTab() const
{
    shaderNodes->getNodeManager()->Render();
}
void EditableOpenNode::DrawCodeEditActionButtons()
{
    if (ImGui::BeginTable("ScriptFileButtons", 2, ImGuiTableFlags_None)) {
        ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableNextRow();
        // Columna izquierda - botón Save
        ImGui::TableSetColumnIndex(0);
        GUI::DrawButton("Save file", IconGUI::SAVE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
            shaderNodes->SaveToFile();
        });
        ImGui::SameLine();
        GUI::DrawButton("Auto fit", IconGUI::NODE_EDITOR_AUTOFIT, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
            shaderNodes->getNodeManager()->Autofit();
        });
        ImGui::SameLine();
        DrawSwitchButton();
        // Columna derecha - botón Close (alineado a la derecha)
        ImGui::TableSetColumnIndex(1);
        GUI::DrawButton("Close file", IconGUI::CLEAR_SCENE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
            Brakeza::get()->GUI()->CloseEditableFile(this);
        });
        ImGui::EndTable();
    }
}

EditableOpenNode::~EditableOpenNode()
{
}
