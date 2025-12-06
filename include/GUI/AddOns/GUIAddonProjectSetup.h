//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
#define BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
#include "../GUI.h"


struct GUIAddonProjectSetup
{
    GUIType::ScriptEditableManager &scriptEditableManager;
public:
    GUIAddonProjectSetup(GUIType::ScriptEditableManager &scriptEditableManager);
    void DrawProjectSetupGUI();
    void DrawProjectScripts() const;
    void DrawSceneScripts() const;
    static void DrawSceneCustomShaders();
};

#endif //BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
