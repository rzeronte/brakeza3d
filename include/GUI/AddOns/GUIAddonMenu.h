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
    void DrawMainMenuWindow();
    void DrawMainMenuBrakeza3D();
    void DrawMainMenuScriptControls();
    void DrawMainMenuAddObject();
    void DrawMainMenuVideo();
    void DrawMainMenuColliders();
    void DrawMainMenuIllumination();
    void DrawMainMenuCamera();
    void DrawMainMenuSound();
    void DrawMainMenuLogging();
    void DrawMainMenuLayout();
    void DrawAboutMeModal();
    static void DrawItemsToLoad(
        const std::string &folder,
        const std::string &extension,
        GUISheet icon,
        const std::function<void(const std::string &)> &callBack
    );
};

#endif //BRAKEZA3D_GUIWIDGETMENU_H
