//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETMENU_H
#define BRAKEZA3D_GUIWIDGETMENU_H

#include "../GUITypes.h"

struct GUIAddonMenu
{
    std::vector<GUITypes::GUIMenuItem> menus;
    std::vector<GUITypes::GUIWindowData> &windowStatus;

    explicit GUIAddonMenu(std::vector<GUITypes::GUIWindowData> &windowStatus);

    void Draw();
    void MenuWindow();
    void MenuBrakeza3D();
    void MenuScriptControls();
    void MenuAddObject();
    void MenuVideo();
    void MenuColliders();
    void MenuIllumination();
    void MenuCamera();
    void MenuSound();
    void MenuLogging();
    void MenuLayout();
    void AboutMeModal();
    static void DrawItemsToLoad(std::string &folder, std::string &ext, GUISheet icon, const std::function<void(const std::string &)> &cb );
};

#endif //BRAKEZA3D_GUIWIDGETMENU_H
