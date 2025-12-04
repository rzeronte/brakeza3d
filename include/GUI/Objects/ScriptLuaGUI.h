//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../3D/Object3D.h"

class GUIManager;

class ScriptLuaGUI
{
public:
    static void DrawPropertiesGUI(ScriptLUA *o);
    static void LoadScriptDialog(GUIManager *gui, const std::string& filename);
    static void DrawScriptsBySelectedObject(GUIManager *gui);
    static void DrawScriptsLuaFolderFiles(GUIManager *gui, std::string& folder);
    static void DrawEditScriptWindow(GUIManager *gui);
    static void DrawObjectVariables(const GUIManager *gui);
    static void DrawGlobalVariables();
    static void drawScriptVariables(GUIManager *gui);

};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H