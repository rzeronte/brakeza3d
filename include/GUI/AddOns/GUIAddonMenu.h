//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETMENU_H
#define BRAKEZA3D_GUIWIDGETMENU_H

#include "../GUI.h"

struct GUIAddonMenu
{
    std::vector<GUIType::MenuItem> menus;
    std::vector<GUIType::WindowData> &windowStatus;

    explicit GUIAddonMenu(std::vector<GUIType::WindowData> &windowStatus);

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

    static void DrawItemsToLoad(std::string &folder, std::string &ext, GUIType::Sheet icon, const std::function<void(const std::string &)> &cb );
};

#endif //BRAKEZA3D_GUIWIDGETMENU_H
