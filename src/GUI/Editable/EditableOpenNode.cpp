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

    auto currentIndexTab = Brakeza::get()->GUI()->getIndexCodeEditorTab();

    ImGuiTabItemFlags flags = (currentIndexTab == tabIndex) ? ImGuiTabItemFlags_SetSelected : 0;
    if (ImGui::BeginTabItem(uniqueTabId.c_str(), nullptr, flags)) {
        DrawCodeEditActionButtons();
        DrawNodeEditorTab();
        ImGui::EndTabItem();
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
