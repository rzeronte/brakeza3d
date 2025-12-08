
#ifndef BRAKEZA3D_SHADERSGUI_H
#define BRAKEZA3D_SHADERSGUI_H

#include <string>

#include "../GUI.h"

class GUIManager;

class ShadersGUI
{
public:
    static void DrawEditShaderWindow(GUIManager *gui);
    static void DrawShaderVariables(GUIManager *gui);
    static void DrawCustomShadersFolder(GUIManager *gui, GUIType::BrowserCache &browser);
    static void DrawShadersBySelectedObject(GUIManager *gui);
    static void LoadShaderDialog(GUIManager *gui, const std::string &folder, const std::string &file);
};


#endif //BRAKEZA3D_SHADERSGUI_H