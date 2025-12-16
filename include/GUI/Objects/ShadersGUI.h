
#ifndef BRAKEZA3D_SHADERSGUI_H
#define BRAKEZA3D_SHADERSGUI_H

#include <string>
#include "../GUI.h"

class GUIManager;

class ShadersGUI
{
public:
    static void DrawEditShaderWindow(GUIManager *gui);
    static void LoadDialogShader(GUIManager *gui);

    static void DrawSourceCodeEdit(GUIManager *gui);

    static void DrawShaderHeader(GUIManager * gui);
    static void DrawShaderConfiguration(GUIManager * gui);
    static void DrawVariableCreator(GUIManager * gui);
    static void DrawVariablesTable(GUIManager * gui);
    static void DrawEmptyStateWarning(GUIManager * gui);
    static void DrawActionButtons(GUIManager * gui);
    static void DrawWinObjectShaders(GUIManager *gui);
    static void LoadDialogShader(GUIManager *gui, const std::string &folder, const std::string &file);
    static std::vector<const char *> GetDataTypeItems(GUIManager *gui);
};


#endif //BRAKEZA3D_SHADERSGUI_H