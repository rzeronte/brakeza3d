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
    static void DrawTypeImGuiControl(ScriptLUATypeData &type, bool showName, bool showIcon);

    static void DrawPropertiesGUI(ScriptLUA *o);
    static void LoadScriptDialog(GUIManager *gui, const std::string& filename);
    static void DrawWinScriptEdition(GUIManager *gui);
    static void DrawWinObjectScripts(GUIManager *gui);

    static void DrawEditScriptWindow(GUIManager *gui);
    static void DrawWinObjectVars(GUIManager *gui);
    static void DrawWinGlobalVars(GUIManager *gui);
    static void DrawScriptHeader(GUIManager * gui);
    static void DrawScriptConfiguration(GUIManager * gui);
    static void DrawVariableCreator(GUIManager * gui);
    static void DrawVariablesTable(GUIManager * gui);
    static void DrawEmptyStateWarning(GUIManager * gui);
    static void DrawSaveButton(GUIManager * gui);

};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H