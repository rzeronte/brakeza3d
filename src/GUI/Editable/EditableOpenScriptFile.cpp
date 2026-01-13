//
// Created by Eduardo on 17/12/2025.
//

#include "../../../include/GUI/Editable/EditableOpenScriptFile.h"

#include "../../../include/Brakeza.h"
#include "../include/GUI/Objects/ScriptLuaGUI.h"

EditableOpenScriptFile::EditableOpenScriptFile(const std::string &tabLabel, const std::string &path, ScriptLUA *script)
:
    EditableOpenFile(tabLabel, path, TextEditor::LanguageDefinition::Lua()),
    script(script)
{
}

[[nodiscard]] ScriptLUA * EditableOpenScriptFile::getScript() const
{
    return script;
}

void EditableOpenScriptFile::setScript(ScriptLUA *shader)
{
    this->script = shader;
}

void EditableOpenScriptFile::DrawEditableOpenFileConfig()
{
    ScriptLuaGUI::DrawScriptConfig(*this);
}

void EditableOpenScriptFile::DrawCodeEditActionButtons()
{
    if (ImGui::BeginTable("ScriptFileButtons", 2, ImGuiTableFlags_None)) {
        ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthFixed);

        ImGui::TableNextRow();

        // Columna izquierda - botón Save
        ImGui::TableSetColumnIndex(0);
        GUI::DrawButton("Save file", IconGUI::SAVE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
            Tools::WriteToFile(getPath(), getEditor().GetText().c_str());
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
