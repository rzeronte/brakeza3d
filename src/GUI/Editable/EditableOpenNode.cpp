//
// Created by Eduardo on 04/01/2026.
//

#include "../../../include/GUI/Editable/EditableOpenNode.h"

#include "../../../include/Brakeza.h"
#include "../../../include/Misc/Tools.h"
#include "../../../include/GUI/Widgets/GUIHelpers.h"


EditableOpenNode::EditableOpenNode(const std::string &uniqueId, const std::string &tabLabel, const std::string &path, ShaderBaseNodes *shaderNodes, bool isLiveShader)
:
    EditableOpenBaseResource(uniqueId, tabLabel, path),
    shaderNodes(shaderNodes),
    isLiveShader(isLiveShader)
{
}

void EditableOpenNode::DrawTabEdition(int tabIndex)
{
    std::string uniqueTabId = getTabLabel() + "##" + getUniqueId();

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
        // Establecer tamaño grande (80% del viewport)
        ImVec2 windowSize = ImVec2(viewport->Size.x * 0.8f, viewport->Size.y * 0.8f);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);
        GUIHelpers::CenterNextWindow();

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
        GUIHelpers::SetupLeftRightColumns();
        ImGui::TableNextRow();
        // Columna izquierda - botones
        ImGui::TableSetColumnIndex(0);
        if (!isLiveShader) {
            GUI::DrawButton("Save file", IconGUI::SAVE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
                shaderNodes->SaveToFile();
            });
            ImGui::SameLine();
        }
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
