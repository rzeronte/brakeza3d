
#ifndef BRAKEZA3D_SHADERSGUI_H
#define BRAKEZA3D_SHADERSGUI_H

#include <string>

class GUIManager;

class ShadersGUI
{
public:
    static void DrawEditShaderWindow(GUIManager *gui);
    static void drawShaderVariables(GUIManager *gui);
    static void drawCustomShadersFolder(GUIManager *gui, std::string folder);
    static void drawSelectedObjectShaders(GUIManager *gui);
    static void LoadShaderDialog(GUIManager *gui, const std::string &folder, std::string &file);
};


#endif //BRAKEZA3D_SHADERSGUI_H