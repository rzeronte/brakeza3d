//
// Created by Eduardo on 16/12/2025.
//

#ifndef BRAKEZA3D_EDITABLEOPENFILE_H
#define BRAKEZA3D_EDITABLEOPENFILE_H
#include <string>

#include "../../Misc/Logging.h"
#include "../../Misc/Tools.h"

class EditableOpenFile
{
    std::string path;
    std::string content;
    TextEditor editor;
    std::string tabLabel;
    bool removed = false;
public:

    explicit EditableOpenFile(const std::string &tabLabel, const std::string &path, const TextEditor::LanguageDefinition &lang)
    :
        path(path),
        tabLabel(tabLabel)
    {
        Logging::Message("[EditableOpenFile] Creating EditableOpenFile: Tab: '%s', File: '%s'", tabLabel.c_str(), path.c_str());

        content = Tools::ReadFile(path.c_str());
        editor.SetLanguageDefinition(lang);
        editor.SetText(content);
        editor.SetReadOnly(false);
    }

    [[nodiscard]] std::string getTabLabel() const
    {
        return tabLabel;
    }

    void setTabLabel(const std::string &tab_label)
    {
        tabLabel = tab_label;
    }

    [[nodiscard]] std::string getPath() const
    {
        return path;
    }

    [[nodiscard]] std::string &getContent() {
        return content;
    }

    void setContent(const std::string &value) {
        content = value;
    }

    void setContentFromFile(const std::string &path)
    {
        Logging::Message("[EditableOpenFile] Updating code editor for file '%s'", path.c_str());
        content = Tools::ReadFile(path.c_str());
        editor.SetText(content);
    }

    [[nodiscard]] TextEditor &getEditor()
    {
        return editor;
    }

    [[nodiscard]] bool isRemoved() const
    {
        return removed;
    }

    void setRemoved(bool value)
    {
        removed = value;
    }

    virtual void DrawEditableOpenFileConfig() = 0;
    virtual void DrawCodeEditActionButtons() = 0;
    virtual ~EditableOpenFile() = default;
};


#endif //BRAKEZA3D_EDITABLEOPENFILE_H