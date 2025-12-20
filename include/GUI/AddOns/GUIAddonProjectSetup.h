//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
#define BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H

class GUIAddonProjectSetup
{
public:
    GUIAddonProjectSetup() = default;

    static void TreeSceneScripts();
    static void TreeProjectScripts();
    static void TreeSceneShaders();

    static void DrawWinProjectSettings();
    static void DrawProjectScripts();
    static void DrawSceneScripts();
    static void DrawSceneCustomShaders();
};

#endif //BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
