
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

    static void DrawEditShaderWindow(EditableOpenShaderFile &file);
    static void DrawShaderHeader(EditableOpenShaderFile &file);
    static void DrawShaderConfiguration(EditableOpenShaderFile &file);
    static void DrawShaderVarsCreator(EditableOpenShaderFile &file);
    static void DrawShaderVarsTable(EditableOpenShaderFile &file);
    static void DrawEmptyStateWarning(EditableOpenShaderFile &file);
    static void DrawShaderActionButtons(EditableOpenShaderFile &file);
    static std::vector<const char *> GetDataTypeItems(EditableOpenShaderFile &file);

    static void DrawWinObjectShaders(GUIManager *gui);
};


#endif //BRAKEZA3D_SHADERSGUI_H