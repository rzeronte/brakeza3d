
#ifndef BRAKEZA3D_SHADERSGUI_H
#define BRAKEZA3D_SHADERSGUI_H

#include <string>
#include "../GUI.h"
#include "../TextEditor/EditableOpenShaderFile.h"

class GUIManager;
class EditableOpenShaderFile;

class ShadersGUI
{
public:
    static void LoadDialogShader(const std::string &folder, const std::string &file);

    static void DrawShaderConfig(EditableOpenShaderFile &file);
    static void DrawShaderConfigHeader(EditableOpenShaderFile &file);
    static void DrawShaderConfigEditName(EditableOpenShaderFile &file);
    static void DrawShaderConfigVarsCreator(EditableOpenShaderFile &file);
    static void DrawShaderConfigVarsTable(EditableOpenShaderFile &file);
    static void DrawShaderConfigEmptyStateWarning(EditableOpenShaderFile &file);
    static void DrawShaderConfigActionButtons(EditableOpenShaderFile &file);
    static std::vector<const char *> GetDataTypeItems(EditableOpenShaderFile &file);

    static void DrawWinObjectShaders(GUIManager *gui);
};


#endif //BRAKEZA3D_SHADERSGUI_H