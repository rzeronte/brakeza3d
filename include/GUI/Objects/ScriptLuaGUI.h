//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../3D/Object3D.h"
#include "../TextEditor/EditableOpenScriptFile.h"

class GUIManager;
class EditableOpenScriptFile;

class ScriptLuaGUI
{
public:
    static void LoadScriptDialog(const std::string& pathFile);

    static void DrawEditScriptWindow(EditableOpenScriptFile &file);
    static void DrawScriptHeader(EditableOpenScriptFile &file);
    static void DrawScriptConfiguration(EditableOpenScriptFile &file);
    static void DrawScriptVarCreator(EditableOpenScriptFile &file);
    static void DrawScriptVarsTable(EditableOpenScriptFile &file);
    static void DrawEmptyStateWarning(EditableOpenScriptFile &file);
    static void DrawActionButtons(EditableOpenScriptFile &file);

    static void DrawTypeImGuiControl(ScriptLUATypeData &type, bool showName, bool showIcon);

    static void DrawWinObjectVars(GUIManager *gui);
    static void DrawWinGlobalVars(GUIManager *gui);
    static void DrawWinObjectScripts(GUIManager *gui);

    static void DrawPropertiesGUI(ScriptLUA *o);
};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H