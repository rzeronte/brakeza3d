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
    static void DrawScriptsLuaFolderFiles(GUIManager *gui, GUIType::FolderBrowserCache &browser);
    static void DrawEditScriptWindow(GUIManager *gui);
    static void DrawObjectVariables(GUIManager *gui);
    static void DrawGlobalVariables(GUIManager *gui);
    static void drawScriptVariables(GUIManager *gui);

};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H