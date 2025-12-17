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

void EditableOpenScriptFile::DrawEditableOpenFileSetup()
{
    ScriptLuaGUI::DrawEditScriptWindow(*this);
}