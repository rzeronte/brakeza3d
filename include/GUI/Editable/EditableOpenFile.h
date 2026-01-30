//
// Created by Eduardo on 16/12/2025.
//

#ifndef BRAKEZA3D_EDITABLEOPENFILE_H
#define BRAKEZA3D_EDITABLEOPENFILE_H

#include <string>

#include "EditableOpenBaseResource.h"
#include "../../Misc/Tools.h"

class EditableOpenFileGUI;

class EditableOpenFile: public EditableOpenBaseResource
{
    std::string content;
    TextEditor editor;
public:

    explicit EditableOpenFile(const std::string &uniqueId, const std::string &tabLabel, const std::string &path, const TextEditor::LanguageDefinition &lang);

    [[nodiscard]] std::string &getContent()         { return content; }
    [[nodiscard]] TextEditor &getEditor()           { return editor; }

    void setContent(const std::string &value);
    void setContentFromFile(const std::string &path);

    void DrawTabEdition(int index) override;

    virtual void DrawEditableOpenFileConfig() = 0;
    virtual void DrawCodeEditActionButtons() = 0;

    ~EditableOpenFile() override;
};

#endif //BRAKEZA3D_EDITABLEOPENFILE_H
