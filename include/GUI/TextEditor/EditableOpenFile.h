//
// Created by Eduardo on 16/12/2025.
//

#ifndef BRAKEZA3D_EDITABLEOPENFILE_H
#define BRAKEZA3D_EDITABLEOPENFILE_H
#include <string>

#include "../../Misc/Tools.h"

class EditableOpenFile
{
    std::string path;
    std::string content;
    TextEditor editor;
    bool removed = false;
public:

    explicit EditableOpenFile(const std::string &path, const TextEditor::LanguageDefinition lang)
    :
        path(path)
    {
        editor.SetLanguageDefinition(lang);

        size_t file_size;
        content = Tools::ReadFile(path.c_str(), file_size);
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

    void setContent(const std::string &content) {
        this->content = content;
    }

    [[nodiscard]] TextEditor &getEditor() {
        return editor;
    }

    [[nodiscard]] bool isRemoved() const {
        return removed;
    }

    void setRemoved(bool removed) {
        this->removed = removed;
    }

    virtual void DrawEditableOpenFileSetup() = 0;
    virtual ~EditableOpenFile() = default;
};


#endif //BRAKEZA3D_EDITABLEOPENFILE_H