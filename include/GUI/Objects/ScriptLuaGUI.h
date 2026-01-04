//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../3D/Object3D.h"
#include "../Editable/EditableOpenScriptFile.h"

class GUIManager;
class EditableOpenScriptFile;

class ScriptLuaGUI
{
public:
    static void LoadScriptDialog(const std::string& pathFile);
    static void DrawScriptConfig(EditableOpenScriptFile &file);
    static int getNumAllowedVars(Object3D *o);

    static void DrawScriptConfigHeader(EditableOpenScriptFile &file);
    static void DrawScriptConfigEditName(EditableOpenScriptFile &file);
    static void DrawScriptConfigVarCreator(EditableOpenScriptFile &file);
    static void DrawScriptConfigVarsTable(EditableOpenScriptFile &file);
    static void DrawScriptConfigEmptyStateWarning(EditableOpenScriptFile &file);
    static void DrawScriptConfigActionButtons(EditableOpenScriptFile &file);
    static void DrawTypeImGuiControl(ScriptLUATypeData &type, bool showName, bool showIcon);
    static void DrawWinObjectVars(GUIManager *gui);
    static void DrawWinGlobalVars(GUIManager *gui);
    static void DrawWinObjectScripts();
    static void DrawPropertiesGUI(ScriptLUA *o);
    static ScriptMetaInfo ExtractScriptMetainfo(const std::string &pathFile);
};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H