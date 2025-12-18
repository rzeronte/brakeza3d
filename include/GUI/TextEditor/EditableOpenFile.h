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
    bool removed = false;
public:

    explicit EditableOpenFile(const std::string &path, const TextEditor::LanguageDefinition &lang)
    :
        path(path)
    {
        editor.SetLanguageDefinition(lang);

        content = Tools::ReadFile(path.c_str());
        editor.SetText(content);
        editor.SetReadOnly(false);
    }

    [[nodiscard]] std::string getPath() const {
        return path;
    }

    void setPath(const std::string &path) {
        this->path = path;
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

    [[nodiscard]] TextEditor &getEditor() {
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