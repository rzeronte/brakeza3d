//
// Created by Eduardo on 16/12/2025.
//

#ifndef BRAKEZA3D_EDITABLEOPENSCRIPTFILE_H
#define BRAKEZA3D_EDITABLEOPENSCRIPTFILE_H

#include "EditableOpenFile.h"
#include "../../Misc/ScriptLUA.h"

class EditableOpenScriptFile : public EditableOpenFile
{
    ScriptLUA *script;
public:
    EditableOpenScriptFile(const std::string &uniqueId, const std::string &tabLabel, const std::string &path, ScriptLUA *script);
    [[nodiscard]] ScriptLUA * getScript() const;
    void setScript(ScriptLUA *shader);
    void DrawEditableOpenFileConfig() override;
    void DrawCodeEditActionButtons() override;
};


#endif //BRAKEZA3D_EDITABLEOPENSCRIPTFILE_H