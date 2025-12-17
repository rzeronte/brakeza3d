//
// Created by Eduardo on 16/12/2025.
//

#ifndef BRAKEZA3D_EDITABLEOPENSCRIPTFILE_H
#define BRAKEZA3D_EDITABLEOPENSCRIPTFILE_H

#include "EditableOpenFile.h"
#include "../../Brakeza.h"
#include "../../Misc/ScriptLUA.h"

class EditableOpenScriptFile : public EditableOpenFile
{
    ScriptLUA *script;
public:
    EditableOpenScriptFile(const std::string &path, ScriptLUA *script);
    [[nodiscard]] ScriptLUA * getShader() const;
    void setShader(ScriptLUA *shader);
    void DrawEditableOpenFileSetup() override;
};


#endif //BRAKEZA3D_EDITABLEOPENSCRIPTFILE_H