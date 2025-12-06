//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
#define BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
#include "../GUIManager.h"

class GUIAddonProjectSetup
{
public:
    GUIAddonProjectSetup();
    static void DrawProjectSetupGUI(GUIManager *gui);
    static void DrawProjectScripts(GUIManager *gui);
    static void DrawSceneScripts(GUIManager *gui);
    static void DrawSceneCustomShaders(GUIManager *gui);
};

#endif //BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
