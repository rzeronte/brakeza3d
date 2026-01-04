//
// Created by Eduardo on 04/01/2026.
//

#include "../../../include/GUI/Editable/EditableOpenFile.h"

#include "../../../include/Misc/Logging.h"
#include "../../../include/GUI/Editable/EditableOpenFileGUI.h"

EditableOpenFile::EditableOpenFile(const std::string &tabLabel, const std::string &path, const TextEditor::LanguageDefinition &lang)
:
    EditableOpenBaseResource(tabLabel, path)
{
    LOG_MESSAGE("[EditableOpenFile] Creating EditableOpenFile: Tab: '%s', File: '%s'", tabLabel.c_str(), path.c_str());

    content = Tools::ReadFile(path.c_str());
    editor.SetLanguageDefinition(lang);
    editor.SetText(content);
    editor.SetShowWhitespaces(false);
    editor.SetReadOnly(false);
}

void EditableOpenFile::setContent(const std::string &value) {
    content = value;
}

void EditableOpenFile::setContentFromFile(const std::string &path)
{
    LOG_MESSAGE("[EditableOpenFile] Updating code editor for file '%s'", path.c_str());
    content = Tools::ReadFile(path.c_str());
    editor.SetText(content);
}


void EditableOpenFile::DrawTabEdition(int index)
{
    EditableOpenFileGUI::DrawCodeEditorTab(*this, index);
}

EditableOpenFile::~EditableOpenFile()
{
}