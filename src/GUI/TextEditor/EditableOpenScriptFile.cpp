//
// Created by Eduardo on 17/12/2025.
//

#include "../../../include/GUI/TextEditor/EditableOpenScriptFile.h"
#include "../include/GUI/Objects/ScriptLuaGUI.h"

EditableOpenScriptFile::EditableOpenScriptFile(const std::string &path, ScriptLUA *script)
:
    EditableOpenFile(path, TextEditor::LanguageDefinition::Lua()),
    script(script)
{
}

[[nodiscard]] ScriptLUA * EditableOpenScriptFile::getShader() const
{
    return script;
}

void EditableOpenScriptFile::setShader(ScriptLUA *shader)
{
    this->script = shader;
}

void EditableOpenScriptFile::DrawEditableOpenFileConfig()
{
    ScriptLuaGUI::DrawScriptConfig(*this);
}

void EditableOpenScriptFile::DrawCodeEditActionButtons()
{
    GUI::DrawButton("Save file", IconGUI::SAVE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
        Tools::WriteToFile(getPath(), getEditor().GetText().c_str());
    });
    ImGui::SameLine();
    GUI::DrawButton("Close file", IconGUI::CLEAR_SCENE, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
        Brakeza::get()->GUI()->CloseEditableFile(this);
    });
}
